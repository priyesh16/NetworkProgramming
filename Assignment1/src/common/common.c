#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <common.h>

/* check if ip address is valid */
int isipaddr(char *addr) {
	struct sockaddr_in sa;
	return inet_pton(AF_INET, addr, &(sa.sin_addr));
}

/* check if the port number is valid */
int isport(unsigned long port) {
	int err = SUCCESS;

	if (port <= 0 || port >= MAXPORTNO){
		printf("port %lu out of range", port); 
		return ERANGE;
	}
	
	return err; 
}

/* given a string input of port number convert to an unsigned long */
void retrieveport(const char* portstr, unsigned long *portno){
	unsigned long port;
	char *endptr;
	int err = SUCCESS;

	port = strtoul(portstr, &endptr, BASE);
	if((err = isport(port)) != 0)
		err_sys("%s \n ", "Invalid Port");
	if (*endptr != '\0')
		printf("Making use of port %lu from %s \n", port, portstr);
	*portno = port;
}

/* print usage */
void usage(char *usage) {
	printf("usage is %s \n", usage);  
}

/* send a termination ack so that the client knows when the 
 * message has ended. The caller dups stdout to socket so we
 * can print the ACK to stdout and it would get written to socket
 */
void sendack(void) {
	fflush(NULL);
	printf("%s", ACK);
	fflush(NULL);
}

/* validate the number of arguments and print useage
 * if invalid
 */
void validatearg(int argc, int max, char *useage){
	if (argc != max){
		usage(useage);
		err_sys("%s \n ", "Invalid No. of Arguments");
	}
}

