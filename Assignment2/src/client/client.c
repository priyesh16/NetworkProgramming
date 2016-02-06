#include <common.h>
#include "cliutil.h"

#define USAGE "./myclient <ip address> <port>"
#define FILENAMESIZE 50

int main(int argc, char *argv[]){
	int sockfd = 0;
	char *buffer;
	tlv_t *bufstp;
	int count = 0;
	char address[10];
	int i;

	userservcnt = atoi(argv[2]);

	buffer = (char *)malloc(MAXBUFSIZE * sizeof(char));
	bzero(buffer, MAXBUFSIZE * sizeof(char)); 

	/* validate address and port provided by the user
	 * and fill in socket structures accordingly
	 */
	validatearg(argc, 3, USAGE);
	getipaddrfromfile(argv[1]); 

	/* open a socket and connect to the server */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	Connect(sockfd, &(fileaddrsp[1].addr), sizeof(struct sockaddr));
	for (i = 0; i < userservcnt; i++) {
		inet_ntop(AF_INET, &(fileaddrsp[i].addr.sin_addr), address, INET_ADDRSTRLEN);
		printf("%s \n", address);
	}
	getfilefromuser();

	Read(sockfd, buffer, MAXBUFSIZE);
	retrievebuffer(buffer, &bufstp);

	Close(sockfd);
	return 0;
}
