#include "cliutil.h"

#define PROMPTSTR "> Print the filename you want to download?(exit to quit)\n>"

int getipaddrfromfile(const char *filename) {
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
		retrieveport(portstr, &port);
		cursockaddrp->sin_port = htons(port);
        	inet_ntop(AF_INET, &(curfileaddr.addr.sin_addr), address, INET_ADDRSTRLEN);
		memcpy(&fileaddrsp[count], &curfileaddr, sizeof(fileaddress_t));
		count++;
		
	}
	userservcnt = count; 
	return SUCCESS;
}


/* return a listen socket bound on  given port */
int getudpsocket(unsigned long port) {
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


void getfilefromuser(int sockfd) {
	char filename[MAXFILENAMESIZE];
	char buffer[MAXBUFSIZE];
	char *end;
	tlv_t *bufstp = NULL;

	while(1) {
		printf(PROMPTSTR);
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
		retrievebuffer(buffer, &bufstp);
printf("\n%d \t%d \t%d %s\t \n", bufstp->buf_type, bufstp->buf_len, 
				bufstp->buf_err, bufstp->buf_error);
		if (bufstp->buf_err == 0) 
			break;
		else {
			printf(FILEINVALIDSTR);
			continue;
		}	
	}
}
