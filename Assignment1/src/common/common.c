#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <common.h>

int isipaddr(char *addr) {
	struct sockaddr_in sa;
	return inet_pton(AF_INET, addr, &(sa.sin_addr));
}

int isport(unsigned long port) {
	int err = SUCCESS;

	if (port <= 0 || port >= MAXPORTNO){
		printf("port %lu out of range", port); 
		return ERANGE;
	}
	
	return err; 
}

int retrieveport(const char* portstr, unsigned long *portno){
	unsigned long port;
	char *endptr;
	int err = SUCCESS;

	port = strtoul(portstr, &endptr, BASE);
	if((err = isport(port)) != 0)
		return err;
	if (*endptr != '\0')
		printf("Making use of port %lu from %s \n", port, portstr);

	*portno = port;
	return err;
}

void usage(char *usage) {
	printf("usage is %s \n", usage);  
}

ssize_t readall(int fd, void *data, size_t count) {
	ssize_t bytesRead;
	char * dataPtr = data;
	size_t total = 0;

	while (count) {
		bytesRead = read(fd, dataPtr, count);
/* we should check bytesRead for < 0 to return
* errors properly, but this is just sample code! */
		dataPtr += bytesRead;
		count -= bytesRead;
		total += bytesRead;
	}
	return total;
}
