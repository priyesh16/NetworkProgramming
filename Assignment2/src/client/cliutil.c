#include "cliutil.h"


int get_ipaddr_list(const char *filename) {
	int fd;
	fileaddress_t curfileaddr = {0};
	struct sockaddr_in *cursockaddrp;	
	int count = 0;
	char line[MAXLINELEN];
	int n;
	char *ipstr;
	char *portstr;
	unsigned long int port;
	struct in_addr ipaddr;
	char address[30];
	int i;

	fd = Open(filename, O_RDONLY, 0);
	fileaddrsp = (fileaddress_t *)malloc(MAXLINES * sizeof(fileaddress_t));
	bzero(fileaddrsp, MAXLINES * sizeof(fileaddress_t));
	while(count < MAXLINES) {
		n = Readline(fd, line, MAXLINELEN);
		if (line[0] ==  NEWLINE)
			continue;
		if (!n)
			break;
		ipstr = strtok(line, " ");
		if (!strcmp(ipstr, LOCALHOSTSTR))
			strcpy(ipstr, LOCALHOST);
		portstr = strtok(NULL, " ");
		
		cursockaddrp = &(curfileaddr.addr); 

		/* Convert ip and port from string to respective types */
		Inet_pton(AF_INET, ipstr, &(cursockaddrp->sin_addr));
		cursockaddrp->sin_family = AF_INET;
		retrieve_port(portstr, &port);
		cursockaddrp->sin_port = htons(port);
        	inet_ntop(AF_INET, &(curfileaddr.addr.sin_addr), address, INET_ADDRSTRLEN);
		memcpy(&fileaddrsp[count], &curfileaddr, sizeof(fileaddress_t));
		count++;
		
	}
	fileservcnt = count; 
	return SUCCESS;
}


/* return a listen socket bound on  given port */
int get_udp_socket(unsigned long port) {
	int sockfd = 0;
	struct sockaddr_in serv_addr; 

	/* fill socket data structure given a port */
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port); 

	/* listen to client and return an accepted socket */
	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	Bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	return sockfd;
}





void check_serv_avail(const char *filename) {
	int sockfd;
	int i = 0;
	int err = SUCCESS;
	char address[MAXADDRSIZE];
	struct	sockaddr_in *addrp;
	for (i = 0; i < fileservcnt; i++) {	
	 	/* open a socket and connect to the server */
		addrp = &(fileaddrsp[i].addr);
		sockfd = Socket(AF_INET, SOCK_STREAM, 0);
		err = connect(sockfd, addrp, sizeof(struct sockaddr));
		inet_ntop(AF_INET, &(addrp->sin_addr), address, INET_ADDRSTRLEN);
		if (err < 0) {			
			printf("Error : Could not connect to address:port %s:%d \n", address, ntohs(addrp->sin_port));
			fileaddrsp[i].servavail = 1;
			continue;
		}
		printf("Success : Connected to address:port %s:%d \n", address, ntohs(addrp->sin_port));
		fileaddrsp[i].sockfd = sockfd;	
	}
}



void send_chunk_info(int serverno, char *buffer, int size, int offset) {
	int err = SUCCESS;
	chunkinfo_t info = {0};	
	int sockfd = fileaddrsp[serverno].sockfd;
	

	info.size = size;
	info.off = offset;
	create_buffer(CHUNKINFO, &info, buffer);
	Write(sockfd, buffer, MAXBUFSIZE);
}


void get_file_status(int serverno) {
	char filename[MAXFILENAMESIZE];
	char buffer[MAXBUFSIZE];
	char *end;
	tlv_t *bufstp = NULL;
	int sockfd = fileaddrsp[serverno].sockfd;

	while(1) {
		printf(PROMPTSTR);
		bzero(buffer, MAXBUFSIZE);
		/* get user input, remove the new line character and
		 * if user doesn't enter a command continue
		 */
		fgets(filename, MAXFILENAMESIZE, stdin);
		snprintf(buffer, MAXFILENAMESIZE, "%s", filename);
		end = strrchr(filename, '\n');
		if (end)
			*end = '\0';
		if (!strlen(filename))
			continue;
		if(!strcmp(filename, EXITCMD))
			exit(1);
		
		Write(sockfd, filename, MAXFILENAMESIZE);
		/* if user enters "exit" then quit, server on receiving
		 * the "exit" command will close the socket and open a
		 * new one for the next client.
		 */
		Read(sockfd, buffer, MAXBUFSIZE);
		retrieve_buffer(buffer, &bufstp);
		printf("1 \n");
		if (bufstp->buf_err == 0)
			break;
		else {
			printf(FILEINVALIDSTR);
			continue;
		}	
	}

}

