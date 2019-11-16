#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int write_crypt()
{
    int fd1;
    char buf[128];

    fd1 = open("/home/cobaia/Documentos/teste.txt", O_WRONLY);
    if (fd1 == -1) {
        return -1;
    }

    scanf("%127s", buf);

    syscall(548, fd1, buf, strlen(buf)); 
    sync();
    close(fd1);
}

int read_crypt()
{
    int fd1, sz;
    char *c = (char *) calloc(127, sizeof(char)); 

    fd1 = open("/home/cobaia/Documentos/teste.txt", O_RDONLY);
    if (fd1 == -1) {
        return -1;
    }

    syscall(549, fd1, c, 32);  
    printf("%s \n",c);
 
    close(fd1);
}

int main (int argc, char *argv[])
{
    int opc;

    do
    {
        printf("Digite 1 para criptar e gravar dado no arquivo. \n");
        printf("Digite 2 para ler e decriptar dado do arquivo. \n");
        printf("Digite 3 para creditos. \n");
        printf("Digite 4 para sair. \n");

        scanf("%i", &opc);

        switch (opc)
        {
        case 1:
            printf("----------------------------------------------------\n");
            printf("Digite o dado que deseja armazenar criptado: \n");
            write_crypt();
            printf("Dado criptografado e armazenado com sucesso! \n");
            printf("----------------------------------------------------\n");
            printf("\n");
            break;

        case 2:
            printf("----------------------------------------------------\n");
            printf("Dado decriptado: \n");
            read_crypt();
            printf("----------------------------------------------------\n");
            printf("\n");
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
            break;
        }

    } while (opc != 4);
    
    
}