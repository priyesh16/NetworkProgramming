#include "server.h"

int main(int argc, char **argv) {
	unsigned long port;
	int sockfd;
	int connfd;
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

	sockfd = create_socket(port);
	/*
	 * connect to a client, accept commands from it
	 * and sent it the output of the command
	 */
		
	while(1){
		/* connect to a new client */
		if (nextclient == 1) {
			connfd = get_listen_socket(sockfd);
			nextclient = 0;
		}
		
		/* Based on the type field of the packet header
		 * handle the clients requests.
		 */
		type = get_type(connfd, buffer);
		if (type == FILENAME)
			get_filename(connfd, buffer);
		if (type == QUERYINFO)
			send_file_status(connfd, buffer);
		if (type == CHUNKINFO)
			get_chunk_info(connfd, buffer, &chunkinfo);
		if (type == CLIENTCLOSE) {
			Close(connfd);
			nextclient = 1;
		}
			
	}

	/* close socket descriptor and exit */
	free(buffer);
	serv_free_all();
	exit(SUCCESS);
}
