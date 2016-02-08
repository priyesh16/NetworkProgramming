#include "server.h"

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
	chunkinfo_t chunkinfo;
	type_t type; 
	int junk = 0;

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
		junk++;
		printf("\n junk %d", junk); 
		if (junk > 3)
			break;
		/* connect to a new client */
		if (nextclient == 1) {
			sockfd = get_listen_socket(port);
			nextclient = 0;
		}
		
		/* read command to be executed from the client, if
		 * read fails for reasons like the client hit Ctrl+C
		 * then connect to next client.
		 */
		type = get_type(sockfd, buffer);
		if (type == FILENAME)
			get_filename(sockfd, buffer);
		if (type == QUERYINFO)
			send_file_status(sockfd, buffer);
		if (type == CHUNKINFO)
			get_chunk_info(sockfd, buffer, &chunkinfo);
	}

	/* close socket descriptor and exit */
	Close(sockfd);
	//free(buffer);
	//free(bufstp);
	exit(SUCCESS);
}
