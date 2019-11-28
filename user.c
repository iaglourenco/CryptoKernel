#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "user.h"

void ascii2hexa(char *string, char hexa[]);
void hexa2ascii(char *string, char ascii[]);
void leitura();
void escrita();


int main ()
{
	int opc;
	
    do
    {
        printf("Digite 1 para criptar e gravar dado no arquivo. \n");
        printf("Digite 2 para ler e decriptar dado do arquivo. \n");
        printf("Digite 3 para creditos. \n");
        printf("Digite 4 para sair. \n");
        scanf("%i", &opc);

        switch (opc){
        case 1:
            escrita();
            break;
        case 2:
			leitura();
            break;
        case 3:
            printf("----------------------------------------------------\n");
            printf("Adriano de Oliveira Munin   17066960 \n");
            printf("Fabio Seiji Irokawa         17057720 \n");
            printf("Iago Jardim Lourenco        15610116 \n");
            printf("Lucas Rodrigues Coutinho    17776501 \n");
            printf("----------------------------------------------------\n");
            printf("\n");
            break;

        case 4:
            break;
        
        default:
            printf("Opcao invalida! \n");
            printf("\n");
	       opc = 4;
            break;
        }

    } while (opc != 4); 
    return 0;
}

void escrita()
{
    
    int fd;
    char bufascii[128];
    char bufhexa[256];
    char criptografado[128];
    int ret;
    int tamFile1=0;
    char *c; 

    fd = open("foo.txt", O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (fd < 0) 
    {
        perror("Falha ao abrir arquivo...");
        return errno;
	}

    printf("----------------------------------------------------\n");
    printf("Digite o dado que deseja armazenar criptado: \n");
	
    scanf("%s", bufascii);
    ascii2hexa(bufascii, bufhexa);
	ret = write_crypt(fd,bufhexa,(strlen(bufascii) * 2));
	if(IS_ERR(ret)){
        perror("Falha ao criptografar... ");
        return errno;
    }
    sync();
	close(fd);

    fd = open("foo.txt", O_RDONLY);

    FILE *stream = fopen("foo.txt","r");
    fseek(stream,0,SEEK_END);
    tamFile1 = ftell(stream);
    fseek(stream,0,SEEK_SET);

    c = (char *) calloc(tamFile1, sizeof(char));
    read(fd, c, tamFile1);
    close(fd);
	fclose(stream);

    printf("Dado armazenadono arquivo: \n");
    printf("%s \n", c);
    
    printf("Dado criptografado e armazenado com sucesso! \n");
    printf("----------------------------------------------------\n");
    printf("\n");
}

void leitura()
{
    int fd;
    int tamFile=0;
    int ret;
    char *c; 
    char *ascii;
    
    fd = open("foo.txt", O_RDONLY);
	if (fd < 0) {
		perror("Falha ao abrir arquivo...");    
	}

    FILE *stream = fopen("foo.txt","r");
    fseek(stream,0,SEEK_END);
    tamFile = ftell(stream);
    fseek(stream,0,SEEK_SET);

    c = (char *) calloc(tamFile, sizeof(char));

    printf("----------------------------------------------------\n");
    printf("Dado decriptado em hexadecimal: \n");
	ret = read_crypt(fd,c,tamFile);
 	if(IS_ERR(ret)){
         perror("Falha na descriptografia... ");
         return errno;
     }
    printf("%s \n",c);
	       
    printf("Dado decriptado em ASCII: \n");
    ascii = malloc(tamFile / 2);
	hexa2ascii(c, ascii); 
	printf("%s \n",ascii);

    printf("----------------------------------------------------\n");
    printf("\n");

    free(ascii);
    close(fd);
    fclose(stream);
}

void ascii2hexa(char *string, char hexa[])
{
    int i;
    int tam = strlen(string);

    for(i = 0; i < tam; i++)
    {        
        sprintf(hexa+i*2,"%02x", string[i]);
    }
    sprintf(hexa+i*2+1,"%c",'\0'); 
}

void hexa2ascii(char *string, char ascii[])
{ 
    int i;    
    int cont = 0;
    int tam = strlen(string);

    char temp[2];
    
    for(i = 0; i < tam; i+=2)
    {         
        temp[0]  = string[i];
        temp[1]  = string[i+1];
        sscanf(temp, "%hhx", &ascii[cont]);
        cont++;    
   }
}

