#include <linux/kernel.h>



asmlinkage ssize_t write_crypt(int fd,const void *buf,size_t nbytes){
	
	printk("Write Crypt Started\n");



	return 0;
}

asmlinkage ssize_t read_crypt(int fd,void *buf,size_t nbytes){
	
	printk("Read Crypt Started\n");


	

	return 0;
}
