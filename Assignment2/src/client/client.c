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
	const char *addrfilename = argv[1];

	userservcnt = atoi(argv[2]);

	buffer = (char *)malloc(MAXBUFSIZE * sizeof(char));
	bzero(buffer, MAXBUFSIZE * sizeof(char)); 

	/* validate address and port provided by the user
	 * and fill in socket structures accordingly
	 */
	validate_arg(argc, 3, USAGE);
	get_ipaddr_list(addrfilename); 
	check_serv_avail(addrfilename);

	get_file_status(fileaddrsp[0].sockfd);

	Close(sockfd);
	return 0;
}
