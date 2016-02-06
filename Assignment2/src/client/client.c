#include <common.h>
#include "cliutil.h"

#define USAGE "./myclient <ip address> <port>"
#define FILENAMESIZE 50

int main(int argc, char *argv[]){
	int sockfd = 0;
	struct sockaddr serv_addrs[MAXLINES]; 
	unsigned int numcon;
	unsigned int servercnt = 0;
	char *buffer;
	tlv_t *bufstp;

	numcon = atoi(argv[2]);

	buffer = (char *)malloc(MAXBUFSIZE * sizeof(char));
	bzero(&serv_addrs, sizeof(serv_addrs)); 

	/* validate address and port provided by the user
	 * and fill in socket structures accordingly
	 */
	validatearg(argc, 3, USAGE);
	getipaddrfromfile(serv_addrs, argv[1], &servercnt); 

	/* open a socket and connect to the server */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	Connect(sockfd, &serv_addrs[0], sizeof(struct sockaddr));
	getfilefromuser();
	
	Read(sockfd, buffer, MAXBUFSIZE);
	retrievebuffer(buffer, &bufstp);

	Close(sockfd);
	return 0;
}
