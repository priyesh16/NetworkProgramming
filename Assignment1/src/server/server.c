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

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port); 

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket():");	
	}

	if (bind(sockfd, (struct sockaddr*)&serv_addr, 
	    sizeof(serv_addr)) == -1) {
		close(sockfd);
		perror("bind():");	
		return errno; 
	}

	if (listen(sockfd, BACKLOG) == -1) { 
		close(sockfd);
		perror("listen():");	
		return errno;
	}

	if ((connfd = accept(sockfd, (struct sockaddr*)NULL,
	    NULL)) == -1){
		close(connfd);
		perror("accept():");	
		return errno; 
	}

	close(sockfd);
	return connfd;
}
