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
	chunkinfo_t chunkinfo;

	buffer = (char *)malloc(MAXBUFSIZE * sizeof(char));
	bzero(buffer, MAXBUFSIZE * sizeof(char));

	userservcnt = atoi(argv[2]);

	/* validate address and port provided by the user
	 * and fill in socket structures accordingly
	 */
	validate_arg(argc, 3, USAGE);
	get_ipaddr_list(addrfilename); 
	check_serv_avail(addrfilename);

	get_file_status(0);
	send_chunk_info(0, buffer, 39, 0);
	Close(sockfd);
	return 0;
}
