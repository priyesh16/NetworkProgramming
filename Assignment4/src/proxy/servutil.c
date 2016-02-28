#include "server.h"

int connect_client(int conn) {
	int err = SUCCESS;
	char address[MAXADDRSIZE];
	struct	sockaddr_in addr;
	struct sockaddr *castaddrp;
	struct sockaddr_in myaddr;
	int sockfd;

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	curconn.clisockfd = sockfd;
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	Bind(sockfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr));

	addr.sin_family = AF_INET;
	memcpy(&(addr.sin_addr), curconn.cliaddr, sizeof(struct sockaddr));
	addr.sin_port = htons(curconn.cliport);
	printf("Connecting to client...(connect might take 1 sec) \n");
	
	castaddrp = (struct sockaddr *)&addr;
	err = connect_timeo(sockfd, castaddrp, sizeof(struct sockaddr), 1);
	inet_ntop(AF_INET, &(addr.sin_addr), address, INET_ADDRSTRLEN);
	if (err < 0) { 		
		printf("Error : Could not connect to address:port %s:%d \n", address, ntohs(addr.sin_port));
		return ERROR;	
	}
	printf("Success : Connected to address:port %s:%d \n", address, ntohs(addr.sin_port));
	return SUCCESS;	
}

