#include "server.h"

int main(int argc, char **argv) {
	unsigned long port;
	int sockfd;
	int nextclient = 1;
	char *buffer;
	chunkinfo_t chunkinfo;
	type_t type; 

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
		/* connect to a new client */
		if (nextclient == 1) {
			sockfd = get_listen_socket(port);
			nextclient = 0;
		}
		
		/* Based on the type field of the packet header
		 * handle the clients requests.
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
	free(buffer);
	serv_free_all();
	exit(SUCCESS);
}
