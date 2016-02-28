#include "common.h"
#include "server.h"

int send_request(int conn) {
	int sockfd;
	char *request;
	int n;

	connect_client(conn);
	sockfd = curconn.clisockfd;
	
	request = curconn.request;
	n = write(sockfd, request, strlen(request));
	if (n <= 0) {
		printf("Write to server failed \n");
		curconn.error = CONN_CLOSED;		
		return ERROR;
	}
	printf("Request \n %s \n", curconn.request);
	n = read(sockfd, curconn.response, MAXBUFSIZE);
	if (n <= 0) {
		printf("Read from client failed \n");
		curconn.error = CONN_CLOSED;		
		return ERROR;
	}
	fflush(NULL);
	return SUCCESS;
}

