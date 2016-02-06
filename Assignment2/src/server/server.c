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
	tlv_t *bufstp;
	char *buffer;
	char buf[10];

	buffer = (char *)malloc(MAXBUFSIZE * sizeof(char));
	bzero(buffer, MAXBUFSIZE * sizeof(char));

	/* validate and retrieve the port from input arguments
	 * and open a socket bound to the port
	 */
	validate_arg(argc, 2, USAGE);
	retrieve_port(argv[1], &port);

	/*
	 * connect to a client, accept commands from it
	 * and sent it the output of the command
	 */
		
	while(1){
		len = sizeof(err);
		err = SUCCESS; 
		/* connect to a new client */
		if (nextclient == 1) {
			sockfd = get_listen_socket(port);
			nextclient = 0;
		}
		
		/* read command to be executed from the client, if
		 * read fails for reasons like the client hit Ctrl+C
		 * then connect to next client.
		 */
		printf("line 3 \n");
		send_file_status(sockfd, buffer);
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
