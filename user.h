
#include <fcntl.h>
#include <unistd.h>



ssize_t write_crypt(unsigned int fd, const char *buf,size_t nbytes){

	return syscall(548, fd, buf, nbytes); 
    
}


ssize_t read_crypt(unsigned int fd, char *buf,size_t nbytes){

	return syscall(549, fd, buf, nbytes); 
}
