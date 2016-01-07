#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#define BACKLOG 1



int main(int argc, char *argv[]){
	int sockfd = 0, connfd = 0, n = 0;
	int ret;
	struct sockaddr_in serv_addr; 
	unsigned long port;

	char sendBuff[1025];
	char recvBuff[1025];
	time_t ticks; 

	if ((ret = retrieveport(argv[1], &port)) != 0)
		return ret;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuff, '0', sizeof(sendBuff)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port); 

	bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

	listen(sockfd, BACKLOG); 

	while(1) {
		connfd = accept(sockfd, (struct sockaddr*)NULL, NULL); 

		ticks = time(NULL);
		while ((n = read(connfd, recvBuff, sizeof(recvBuff)-1)) > 0){
			recvBuff[n] = 0;
			if(fputs(recvBuff, stdout) == EOF){
				printf("\n Error : Fputs error\n");
			}
		}
		printf("got %s \n", recvBuff);
		snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
		write(connfd, sendBuff, strlen(sendBuff)); 

		close(connfd);
		sleep(1);
	}
	close(connfd);
}
