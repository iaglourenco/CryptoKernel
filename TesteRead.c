#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
    int fd1, sz;
    char *c = (char *) calloc(127, sizeof(char)); 

    fd1 = open("/home/cobaia/Documentos/teste.txt", O_RDONLY);
    if (fd1 == -1) {
        return EXIT_FAILURE;
    }

    syscall(549, fd1, c, 32);  
    printf("%s \n",c);
 
    close(fd1);
    return 0;
}

