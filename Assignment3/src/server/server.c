#include "server.h"


int main(int argc, char **argv) {
	unsigned long port;
	int nextclient = 1;
	type_t type = 20; 
	
	/* Dynamically allocated memory that will be used 
	 * throughout the program
	 */
	mallocgp();
	/* validate and retrieve the port from input arguments
	 * and open a socket bound to the port
	 */
	validate_arg(argc, 2, USAGE);
	retrieve_port(argv[1], &port);
<<<<<<< HEAD
	
=======

>>>>>>> 11e74c0cdc029b8f29f6e56cfb8c95f1a1251990
	create_socket(port);
	/* connect to a client, accept commands from it
	 * and sent it the output of the command
	 */
		
	while(1){
		bzero(buffer, MAXBUFSIZE * sizeof(char));
		/* connect to a new client */
		if (nextclient == 1) {
			//connfd = get_listen_socket(sockfd);
			nextclient = 0;
		}
		
		/* Based on the type field of the packet header
		 * handle the clients requests.
		 */

		type = get_type();
		if (type == SENDACK)
			send_ack(sockfd, buffer, destaddrgp);
		if (type == FILENAME)
			get_filename();
		if (type == QUERYINFO)
			send_file_status();
		if (type == CHUNKINFO)
			get_chunk_info();
		if (type == CLIENTCLOSE)
			nextclient = 1;

	}

	/* close socket descriptor and exit */
	freegp();
	serv_free_all();
	exit(SUCCESS);
}
