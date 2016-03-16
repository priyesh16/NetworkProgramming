#include "router.h"

int main(int argc, char **argv) {
	short port;
	int conn;
	int sockfd;
	int maxcon = 0;
	int connarr[MAXCONNECTIONS];
	int i;
	fd_set readfd;
	struct timeval tv;
	int maxfd;
	
	/* Validate and retrieve the port from input arguments
	 * and open a socket bound to the port
	 */
	validate_arg(argc, 3, USAGE);
	retrieve_port(argv[1], &port);
	
	// Open file for logging
	start_log(argv[2]);	
	
	// Read node.config file
	read_node_config();
	
	sockfd = create_socket(port);
	/* Accept multiple concurrent persistant requests
	 * from multiple client/browsers. Multiple read 
	 * requests are handled using select().
	 */
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	maxfd = sockfd + 1;

	if (pthread_mutex_init(&lock, NULL) != 0) {
		printf("\n mutex init failed \n");
		exit(1);
	}
	
	while(1) {
		FD_ZERO(&readfd);
		FD_SET(sockfd, &readfd);
		Select(maxfd, &readfd, NULL, NULL, &tv);
		if (FD_ISSET(sockfd, &readfd)) {
			conn = get_listen_socket(sockfd);
			if (conn == -1) {
				send_error(conn);
				continue;
			}
			connection[maxcon].servsockfd = conn;
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
	pthread_mutex_destroy(&lock);

	exit(SUCCESS);
}

void *thread_func(void *connp) {
	int conn = *(int *)connp;

	if (!read_packet(conn)) {
		send_error(conn);
	}
	return NULL;
}

int send_request(int conn) {
	int sockfd;
	char *request;
	int n;

	connect_client(conn);
	sockfd = curconn.clisockfd;
	request = curconn.request;

	/* Send request to the server, if server
	 * closes the connection, handle it.
	 */
	n = write(sockfd, request, strlen(request));
	if (n <= 0) {
		printf("Write to server failed \n");
		curconn.error = CONN_CLOSED;		
		return ERROR;
	}
	printf("Request \n %s \n", curconn.request);

	/* Read response from the server, if server
	 * closes the connection, handle it.
	 */
	n = read(sockfd, curconn.response, MAXBUFSIZE);
	if (n <= 0) {
		printf("Read from client failed \n");
		curconn.error = CONN_CLOSED;		
		return ERROR;
	}
	fflush(NULL);
	return SUCCESS;
}

