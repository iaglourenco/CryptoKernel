#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main (int argc, char *argv[])
{



    int fd1;
    char buf[128];
    char *c = (char *) calloc(127, sizeof(char)); 
    int op = -1;

    do
    {

        printf("Write e Read com criptografia em arquivo usando syscall\n");
        printf("Use os comandos abaixo se deseja escrever ou ler o arquivo\n");
        printf("1 - Escrever\n2 - Ler\n3 - Sair\n");
        scanf("%i",op);

        if (op == 1)
        {
            fd1 = open("teste.txt", O_WRONLY);
            if (fd1 == -1) 
            {
                return EXIT_FAILURE;
            }

            scanf("%127s", buf);

            //syscall(548, fd1, buf, strlen(buf)); 
            int sss = strlen(buf);	
            printf("Sent %i, %d and %i\n",fd1,*buf,sss);
            write(fd1,buf,strlen(buf));
            close(fd1);
        }

        else if(op == 2)
        {
            fd1 = open("teste.txt", O_RDONLY);
            if (fd1 == -1) 
            {
                return EXIT_FAILURE;
            }
            //syscall(549, fd1, c, 127);
            read(fd1, c, 127); 
            printf("%s",c);
            close(fd1);
        }

    }
    while(op != 3);
    return 0;
}