#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <poll.h>

#define PROMPT ">"
#define POLLTIMEOUT 10000

int main(int argc, char *argv[]){
	int sockfd = 0, n = 0;
	int ret;
	char sendBuff[1024];
	char recvBuff[1024];
	struct sockaddr_in serv_addr; 
	unsigned long port;
	char cmd[100];
	struct pollfd pollst[1];
	char *end = NULL;

	if(argc != 3){
		printf("\n Usage: %s <ip of server> \n",argv[0]);
		return 1;
	}  

	if ((ret = retrieveport(argv[2], &port)) != 0) 
		return ret;

	memset(recvBuff, '0',sizeof(recvBuff));
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\n Error : Could not create socket \n");
		return 1;
	} 

	pollst[0].fd = sockfd;
	pollst[0].events = POLLIN;

	memset(&serv_addr, '0', sizeof(serv_addr)); 
	memset(&sendBuff, '0', sizeof(sendBuff)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port); 

	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
		printf("\n inet_pton error occured\n");
		return 1;
	} 

	if(connect(sockfd, (struct sockaddr *)&serv_addr, 
		    sizeof(serv_addr)) < 0) {
		printf("\n Error : Connect Failed \n");
		return 1;
	} 

	while(1){
		printf(PROMPT);
		fgets(cmd, sizeof(cmd), stdin);
		end = strrchr(cmd, '\n');
		if (end)
			//printf("end %s", end);
			*end = '\0';
		if (!strlen(cmd))
			continue;
		printf("%s", cmd);
		snprintf(sendBuff, strlen(cmd) + 1, "%s", cmd);
		n = write(sockfd, sendBuff, strlen(sendBuff));
		ret = poll(pollst, 1, POLLTIMEOUT);
		if (pollst[0].revents | POLLIN){	
			n = read(sockfd, recvBuff, sizeof(recvBuff)-1);
			recvBuff[n] = 0;
		}
		printf("\t\t\tServer Output : \n");
		printf("%s \n", recvBuff);
	} 
	return 0;
}
