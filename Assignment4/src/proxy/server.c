#include "server.h"


int main(int argc, char **argv) {
	short port;
	int connfd;
	fd_set readfd;
	struct timeval tv;
	int maxfd;
	int maxcon;
	int connarr[MAXCONNECTIONS];
	int i;
	
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	
	
	/* Dynamically allocated memory that will be used 
	 * throughout the program
	 */
	mallocgp();
	/* validate and retrieve the port from input arguments
	 * and open a socket bound to the port
	 */
	validate_arg(argc, 2, USAGE);
	retrieve_port(argv[1], &port);
	
	sockfd = create_socket(port);
	maxfd = sockfd + 1;
	/* connect to a client, accept commands from it
	 * and sent it the output of the command
	 */
		
	while(1) {
		FD_ZERO(&readfd);
		FD_SET(sockfd, &readfd);
		Select(maxfd, &readfd, NULL, NULL, &tv);
		if (FD_ISSET(sockfd, &readfd)) {
			connfd = get_listen_socket(sockfd);
			maxfd = connfd + 1;
			connection[maxcon].servsockfd = connfd;
			connarr[maxcon] = maxcon;
			Pthread_create(&(connection[maxcon].thread), NULL,
				&thread_func, (&connarr[maxcon]));
			maxcon++;
		}
	}
	for (i = 0; i < maxcon; i++) 
		Pthread_join(connection[i].thread, NULL);
	/* close socket descriptor and exit */
	freegp();
	serv_free_all();
	exit(SUCCESS);
}

void *thread_func(void *connp) {
	int conn = *(int *)connp;
		
	myprintf("Reading packet ...\n");
	bzero(buffer, MAXBUFSIZE * sizeof(char));
	if (!read_packet(conn))
		send_error(conn);
	return NULL;
}
