#include "client.h"

int main(int argc, char **argv) {
	short port;
	int connfd;
	int sockfd;
	fd_set readfd;
	struct timeval tv;
	int maxfd;
	int maxcon;
	int connarr[MAXCONNECTIONS];
	int i;
	
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	/* Validate and retrieve the port from input arguments
	 * and open a socket bound to the port
	 */
	validate_arg(argc, 3, USAGE);
	retrieve_port(argv[1], &port);
	
	// Open file for logging
	start_log(argv[2]);	
	
	sockfd = create_socket(port);
	maxfd = sockfd + 1;
	/* Accept multiple concurrent persistant requests
	 * from multiple client/browsers. Multiple read 
	 * requests are handled using select().
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
			// Each client is executed by a separate thread
			Pthread_create(&(connection[maxcon].thread), NULL,
				&thread_func, (&connarr[maxcon]));
			maxcon++;
			// End of marker for one request
			myprintf(0, "\n*****************************************\n");
		}
	}
	for (i = 0; i < maxcon; i++) 
		Pthread_join(connection[i].thread, NULL);
	/* close socket descriptor and exit */

	exit(SUCCESS);
}

void *thread_func(void *connp) {
	int conn = *(int *)connp;
		
	if (!read_packet(conn)) {
		send_error(conn);
	}
	return NULL;
}
