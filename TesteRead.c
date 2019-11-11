#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
    int fd1, sz;
    char *c = (char *) calloc(127, sizeof(char)); 

    fd1 = open("abc.txt", O_RDONLY);
    if (fd1 == -1) {
        return EXIT_FAILURE;
    }

    sz = read(fd1, c, 127); 
    c[sz] = '\0'; 
    printf("%s",c);
 
    close(fd1);
    return 0;
}

