#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <common.h>

#define BACKLOG 1

/* return a listen socket bound on  given port */
int getlistensocket(unsigned long port) {
	int sockfd = 0, connfd = 0;
	int ret;
	struct sockaddr_in serv_addr; 

	/* fill socket data structure given a port */
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port); 

	/* listen to client and return an accepted socket */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	Bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	Listen(sockfd, BACKLOG);
	connfd = Accept(sockfd, (struct sockaddr*)NULL, NULL);

	/* close listening socket and return the client socket */
	close(sockfd);
	return connfd;
}

void getfilename(int sockfd, char *buffer) {
	Read(sockfd, buffer, MAXBUFSIZE);	
}

