
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "user.h"

int main ()
{
	int opc;
	char buf[128];
	char *c; 
	int fd=creat("./teste.txt",S_IRWXU);
    FILE *stream = fopen("./teste.txt","r");
    int tamFile=0;
	close(fd);
    do
    {
        printf("Digite 1 para criptar e gravar dado no arquivo. \n");
        printf("Digite 2 para ler e decriptar dado do arquivo. \n");
        printf("Digite 3 para creditos. \n");
        printf("Digite 4 para sair. \n");

        scanf("%i", &opc);

        switch (opc){

        case 1:
            fd = open("./teste.txt",O_WRONLY);
			if (fd < 0) {
				perror("Falha ao abrir arquivo...");
			    return errno;     
			}
            printf("----------------------------------------------------\n");
            printf("Digite o dado que deseja armazenar criptado: \n");
			scanf("%127s", buf);
            write_crypt(fd,buf,strlen(buf));
			sync();
			close(fd);
            printf("Dado criptografado e armazenado com sucesso! \n");
            printf("----------------------------------------------------\n");
            printf("\n");
            break;

        case 2:
			fd = open("./teste.txt",O_RDONLY);
			if (fd < 0) {
				perror("Falha ao abrir arquivo...");
			    return errno;     
			}
            fseek(stream,0,SEEK_END);
            tamFile = ftell(stream);
            fseek(stream,0,SEEK_SET);
            c = (char *) calloc(127, sizeof(char));
            printf("----------------------------------------------------\n");
            printf("Dado decriptado: \n");
			read_crypt(fd,c,tamFile);
 			printf("%s \n",c);
    		close(fd);
            fclose(stream);
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
