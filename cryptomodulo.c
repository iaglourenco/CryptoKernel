#include <linux/init.h>         //Funçoes __init, __exit 
#include <linux/module.h>       //Necessario pra qualquer modulo de kernel
#include <linux/device.h>       // Suporte para modulos de dispositivos
#include <linux/kernel.h>       //macros do kernel
#include <linux/fs.h>           // Suporte ao sistema de arquivos linux
#include <linux/uaccess.h>      //Função copy_to_user
#include <linux/crypto.h>       //Funçoes de criptografia
#include <crypto/skcipher.h>    //Funçoes de criptografia
#include <linux/mutex.h>
#include <linux/scatterlist.h>
#include <crypto/internal/hash.h>
#include <linux/vmalloc.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adriano Munin, Fabio Irokawa, Iago Lourenço, Lucas Coutinho");
MODULE_DESCRIPTION("Modulo de criptografia");
MODULE_VERSION("0.1");

/* Struct que guarda o resultado da cripto ou descriptografia */ 
struct tcrypt_result {
    struct completion completion;
    int err;
};
/* Junção com todas as structs utilizadas pelas funçoes de cryptografia */
struct skcipher_def {
    struct scatterlist sg[3];
    struct crypto_skcipher *tfm;
    struct skcipher_request *req;
    struct tcrypt_result result;
    struct crypto_wait wait;
};

char *key,*iv,*tempIv,*tempKey;
char *ivC = "0123456789ABCDEF";
char *keyC = "0123456789ABCDEF"; //Guarda o array de strings recebidos do usuario
//static char cryptokey[32];
//static char cryptoiv[32];

static DEFINE_MUTEX(crypto_mutex);
static int tamInput;
static char *encrypted;
static int tamSaida; 
static char *decrypted;
//static int tamSaida;
static char hash[42]={0};
static char hashAscii[41]={0};
//static int tamSaida;
static char *ivLocal;
int pos,i;
char op;
char buf;

module_param(iv,charp,0000);
MODULE_PARM_DESC(iv,"Vetor de inicialização");

module_param(key,charp,0000);
MODULE_PARM_DESC(key,"Chave de criptografia");

//Prototipo das funçoes
static ssize_t inicio(const char *buffer,size_t len);
static void init_cifra(char *msgInput, char *msgOutput, int opc);
static void ascii2hexa(unsigned char *in, char *out, int len);
static int unpadding(char *string, int len);
static void padding(char *string, int len);

int converteASCII(char *string, char *ascii){ 
    char temp[2];
    int i;    
    int cont = 0;
    int tam = strlen(string);
    for(i = 0; i < tam; i+=2){         
        temp[0]  = string[i];
        temp[1]  = string[i+1];
        sscanf(temp, "%hhx", &ascii[cont]);
        cont++;    
   }
   return 1;
}

static ssize_t inicio(const char *buffer,size_t len){

    char temp[2];
    char *ascii;
    char *input;    
    char blocoIn[16]={0};
    char blocoCrypto[16]={0};
    int cont = 0, indice;


    op = buffer[0];
    tamInput = len - 1;
    if(tamInput%32 && op == 'd') return -1;//Caso a entrada nao seja multiplo de 32, retorna erro
        
    if(!(tamInput % 16)){
        input = vmalloc(tamInput + 32);
    }else{
        input = vmalloc(tamInput);    
    }

    if(!input){
        printk(KERN_ERR "kmalloc(input) failed\n");
        return -ENOMEM;
    }

    if(!(tamInput % 16))
        ascii = vmalloc(tamInput/2 + 16);
    else
        ascii = vmalloc(tamInput/2);
    if (!ascii) {
        printk(KERN_ERR  "kmalloc(ascii) failed\n");
        return -ENOMEM;
    }

    ivLocal = vmalloc(16);
    if (!ivLocal) {
        printk(KERN_ERR  "kmalloc(input) failed\n");
        return -ENOMEM;
    }    

    memcpy(ivLocal, ivC, 16);
    memcpy(input, buffer+1,tamInput);

    if(op == 'c') {
        padding(input, tamInput); //Caso a opcao seja de criptgrafia, o padding eh feito na entrada.
        tamInput += 32 - (tamInput%32); //Atualiza o tamanho do texto apos o padding    
    } 
   
    //Conversao de hexa para ascii
    for(indice = 0; indice < tamInput; indice+=2){
        temp[0]  = input[indice];
        temp[1]  = input[indice+1];
        sscanf(temp, "%hhx", &ascii[cont]);
        cont++;    
    }

    if(op == 'c'){
        printk("CRYPTO--> Criptografando..\n"); 
        //Aqui entra a criptografia!

        for(indice = 0; indice < cont/16; indice++){//Cont tem a qtd de caracteres ascii, sempre multiplo de 16 (padding)            
            for(i = 0; i < 16; i++){//Copia um bloco para criptografar
                blocoIn[i] = ascii[indice*16 + i];//Indice*16 para deslocar o bloco (Indice tem o num. do bloco)
            }
            init_cifra(blocoIn, blocoCrypto, 1);
            
            for(i = 0; i < 16; i++){//Copia um bloco criptografado 
                ascii[indice*16 +i] = blocoCrypto[i];//Como o bloco atual de ascii ja foi criptografado, ele eh sobrescrito
            }
        }

        encrypted=vmalloc(cont*2+1);
        if(!encrypted){
            printk(KERN_ERR "kmalloc(encrypted) error");
        }

        ascii2hexa(ascii, encrypted, cont);//ascii tem todos os blocos criptografados
        tamSaida = cont*2;
        encrypted[cont*2] = '\0';

    }else if(op == 'd'){
        if(tamInput%32) return -1;//Caso a entrada nao seja multiplo de 32, retorna erro
        printk("CRYPTO--> Descriptografando..\n"); 
        //descriptografia aqui

        for(indice = 0; indice < cont/16; indice++){//Cont tem a qtd de caracteres ascii, sempre multiplo de 16 (padding)            
            for(i = 0; i < 16; i++){//Copia um bloco para criptografar
                blocoIn[i] = ascii[indice*16 + i];//Indice*16 para deslocar o bloco (Indice tem o num. do bloco)
            }

            init_cifra(blocoIn, blocoCrypto, 2);
            
            for(i = 0; i < 16; i++){//Copia um bloco criptografado 
                ascii[indice*16 +i] = blocoCrypto[i];//Como o bloco atual de ascii ja foi criptografado, ele eh sobrescrito
            }
        }

        decrypted=vmalloc(cont*2);
        if(!decrypted){
            printk(KERN_ERR "kmalloc(encrypted) error");
        }

        ascii2hexa(ascii, decrypted, cont);
        tamSaida=cont*2;

        if(unpadding(decrypted, tamSaida) == 0)//Na descriptografia o unpadding eh feito na saida         
            return -1;                         //Retorna erro se nao tiver padding valido 

       // printk("DEBUG HEX2ASC %s\n", decrypted);
    }else{
        printk("CRYPTO--> Gerando Hash..\n");
        //hash aqui
        //init_hash(ascii, hashAscii, cont);
        
        ascii2hexa(hashAscii, hash, 40);
        tamSaida = 40;
        hash[40]='\0';
    }

    printk(KERN_INFO "CRYPTO-->  Recebida mensagem com %ld caracteres!\n", len -1);
    vfree(ascii);
    vfree(input);
    vfree(ivLocal);
    return len;
}

static void ascii2hexa(unsigned char *in, char *out, int len){
    int i = 0;
    while (i < len){        
        sprintf(out+i*2, "%02x", *in++);
        i++;       
    }
}

static void init_cifra(char *msgInput, char *msgOutput, int opc){
        /* local variables */
        struct skcipher_request *req ;
        struct crypto_skcipher *skcipher = NULL;
        struct skcipher_def sk;
        int ret, i;
        char saida[16];
        char entrada[16];

        skcipher = crypto_alloc_skcipher("ecb(aes)", 0, 0);

        req = skcipher_request_alloc(skcipher, GFP_KERNEL);
        if (req == NULL) {
                printk("failed to load transform for aes");
                goto out;
        }

        ret = crypto_skcipher_setkey(skcipher, keyC, strlen(keyC));
        if (ret) {
                printk(KERN_ERR  "setkey() failed\n");
                goto out;
        }

        skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG,
            crypto_req_done,
            &sk.wait);        

        for(i = 0; i < 16; i++){
            entrada[i] = msgInput[i];
        } 

        sk.tfm = skcipher;
        sk.req = req;

        sg_init_one(&sk.sg[0], entrada, 16);
        sg_init_one(&sk.sg[1], saida, 16);

        if(opc == 1){  
            skcipher_request_set_crypt(req, &sk.sg[0], &sk.sg[1], 16, ivLocal);
            crypto_init_wait(&sk.wait);
            init_completion(&sk.result.completion);
        
            ret = crypto_wait_req(crypto_skcipher_encrypt(sk.req), &sk.wait);
            if (ret) {
                printk(KERN_ERR  "encryption failed erro");
                goto out;
            }
        }else{
            skcipher_request_set_crypt(req, &sk.sg[0], &sk.sg[1], 16, ivLocal);
            crypto_init_wait(&sk.wait);
            init_completion(&sk.result.completion);
        
            ret = crypto_wait_req(crypto_skcipher_decrypt(sk.req), &sk.wait);
            if (ret) {
                printk(KERN_ERR  "encryption failed erro");
                goto out;
            }
        }

    for(i = 0; i < 16; i++){
        msgOutput[i] = saida[i];
    }

    
out:
    if (skcipher)
        crypto_free_skcipher(skcipher);
    if (req)
        skcipher_request_free(req);       
}


static void padding(char *string, int len){ //Padrao utilizado PKCS#7
    int qdtBlocos32, bytesOcupados;
    int i;
    qdtBlocos32 = len/32;   //Obtem a quantidade de blocos completos
    bytesOcupados = len%32; //Obtem a quantidade de bytes usados no ultimo bloco

    if(bytesOcupados == 0){ //Caso a string tenha o tamanho multiplo de 16, preenche um novo blco com o num 0x10 (tamanho do bloco)
        for(i = 0; i < 32; i++){            
            sprintf(string + qdtBlocos32*32 + i*2,"%02x", 16);//Converte 16 decimal para hexa (0x10)
        }
    }

    else {
        for(i = 0; i < (32 - bytesOcupados); i++){//O ultimo bloco eh preenchido com o valor da qtd de bytes livres
            sprintf(string + qdtBlocos32*32 + i*2 + bytesOcupados,"%02x", (32 - bytesOcupados)/2);
         }
    }    
}

static int unpadding(char *string, int len){ //Padrao utilizado PKCS#7
    char temp[3];
    int qtdPadding;//Quantidade de bytes usados no padding
    int numP;//Numero usado para preencher o padding
    int i;

    temp[0]  = string[len-2];//Ultimo numero sempre eh usado para calcular o padding
    temp[1]  = string[len-1];
    temp[2]  = '\0';    
    sscanf(temp, "%x", &qtdPadding);// Converte o num de hexa para decimal

    for(i = 0; i < qtdPadding*2; i += 2){
        temp[0]  = string[len - 2 - i];
        temp[1]  = string[len - 1 - i];
        temp[2]  = '\0';
        sscanf(temp, "%x", &numP);
        if(numP != qtdPadding){//Caso o numero usado para preencher seja diferente da qtd, retorna erro
            printk("Erro de padding\n");
            return 0; 
        } 
    }
    string[len - qtdPadding*2] = '\0';//Descarta numeros usados no padding
    return 1;
}

int init_module(void)
{
    char buffer_encrypt[] = "c123abc";
    char buffer_decrypt[] = "d2cd1bf1a40edc92d5e9dfcbe1514e150";

    inicio(buffer_encrypt, 7);
    printk("String Criptografada: %s", encrypted);
    
    inicio(buffer_decrypt, 33);
    printk("String Descriptografada: %s", decrypted);

    return 0;
}
void cleanup_module(void)
{
    printk("Goodbye world 1.\n");
}