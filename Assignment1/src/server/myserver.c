#include <stdio.h>
#include <errno.h>
#include <common.h>

#define BASE 10
#define USAGE "\n ./myserver <port no (1-65535)> \n"
#define SUCCESS 0

int validatearg(int argc){

	if (argc != 2){
		printf("%s \n ", "Invalid No. of Arguments");
		usage(USAGE);
		return EINVAL;
	}
	
	return SUCCESS;
}

int main(int argc, char **argv){
	int err = SUCCESS; 
	unsigned long port;

	if ((err = validatearg(argc)) != 0)
		return err;

	if ((err = retrieveport(argv[1], &port)) != 0)
		return err;
}
