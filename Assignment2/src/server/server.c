#include <common.h>

#define USAGE "\n ./myserver <port no (1-65535)> \n"

int main(int argc, char **argv) {
	int ret = SUCCESS; 
	int err = SUCCESS; 
	unsigned long port;
	int sockfd;
	int saved_stdout;
	FILE *dummyfp;
	ssize_t n;
	off_t *sizep;
	int nextclient = 1;
	socklen_t len = sizeof(err);
	struct stat st = {0};
	tlv_t *bufstp;
	char *buffer;
	char buf[10];

	buffer = (char *)malloc(MAXBUFSIZE * sizeof(char));

	/* validate and retrieve the port from input arguments
	 * and open a socket bound to the port
	 */
	validatearg(argc, 2, USAGE);
	retrieveport(argv[1], &port);

	/*
	 * connect to a client, accept commands from it
	 * and sent it the output of the command
	 */
		
	while(1){
		len = sizeof(err);
		err = SUCCESS; 
		/* connect to a new client */
		if (nextclient == 1) {
			sockfd = getlistensocket(port);
			nextclient = 0;
		}

		/* read command to be executed from the client, if
		 * read fails for reasons like the client hit Ctrl+C
		 * then connect to next client.
		 */
		getfilename(sockfd, buffer);
		printf("%s", buffer);
		err = stat(buffer, &st);
		if (err != 0) {
			printf("4 %d \n", errno);
			createbuffer(FILEERROR, &errno, buffer);
		}
		else 
			createbuffer(FILESIZE, &(st.st_size), buffer); 
		retrievebuffer(buffer, &bufstp); 
		Write(sockfd, buffer, MAXBUFSIZE);
		break;
		


		/* print command output on server side also */

		/* instead of writing output to stdout write 
		 * directly to socket and then send an ack
		 */
	}

	/* close socket descriptor and exit */
	Close(sockfd);
	free(buffer);
	free(bufstp);
	exit(SUCCESS);
}
