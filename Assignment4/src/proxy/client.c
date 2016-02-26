#include "common.h"
#include "cliutil.h"

#define USAGE "./myclient <file name> <no of connections>"
#define FILENAMESIZE 50

int send_request(int conn) {
	int sockfd;
	char *request;
	printf("request3\n %s \n ", curconn.request);
	connect_client(conn);
	sockfd = curconn.clisockfd;
	
	request = curconn.request;
	printf("request1 \n %s \n ", curconn.request);
	printf("request1 \n %s \n ", curconn.request);
	
	Write(sockfd, request, strlen(request));
	printf("request \n %s \n ", curconn.request);
	Read(sockfd, curconn.response, MAXBUFSIZE);
	printf("response \n %s \n ", curconn.response);	
	Write(curconn.servsockfd, curconn.response, MAXBUFSIZE);
	return SUCCESS;

}

