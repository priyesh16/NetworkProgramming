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
	fileservcnt = count++;
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

void check_serv_avail() {
	int sockfd;
	int i = 0;
	int err = SUCCESS;
	char address[MAXADDRSIZE];
	int count = 0;
	struct	sockaddr_in *addrp;
	for (i = 0; i < fileservcnt; i++) {	
	 	/* open a socket and connect to the server */
		addrp = &(fileaddrsp[i].addr);
		sockfd = Socket(AF_INET, SOCK_STREAM, 0);
		err = connect(sockfd, addrp, sizeof(struct sockaddr));
		inet_ntop(AF_INET, &(addrp->sin_addr), address, INET_ADDRSTRLEN);
		if (err < 0) {			
			printf("Error : Could not connect to address:port %s:%d \n", address, ntohs(addrp->sin_port));
			fileaddrsp[i].servavail = 0;
			continue;
		}
		fileaddrsp[i].servavail = 1;
		printf("Success : Connected to address:port %s:%d \n", address, ntohs(addrp->sin_port));
		fileaddrsp[i].sockfd = sockfd;	
		printf("check_serv %d \n", fileaddrsp[i].sockfd);		
		count++;
	}
	availservcnt = count; 
}



void send_chunk_info(int serverno) {
	int err = SUCCESS;
	int sockfd = fileaddrsp[serverno].sockfd;
	chunkinfo_t *infop = &(fileaddrsp[serverno].chunkinfo);
	printf("send_chunk_info sockfd %d %d \n", serverno, fileaddrsp[serverno].sockfd);
	printf("send_chunk_info %d %ld %ld \n", serverno, fileaddrsp[serverno].cksize, fileaddrsp[serverno].ckoff);
	create_buffer(CHUNKINFO, infop, buffer);
	Write(sockfd, buffer, MAXBUFSIZE);
}

void send_query_info(int serverno) {
	int err = SUCCESS;
	int sockfd = fileaddrsp[serverno].sockfd;
	chunkinfo_t *infop = &(fileaddrsp[serverno].chunkinfo);
	printf("send_query_info sockfd %d %d \n", serverno, fileaddrsp[serverno].sockfd);

	create_buffer(QUERYINFO, infop, buffer);
	Write(sockfd, buffer, MAXBUFSIZE);
}


void get_file_from_user() {
	char buffer[MAXBUFSIZE];
	char *end;
	tlv_t *bufstp = NULL;
	
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
		printf("getfilefromuser %s \n", filename);
		break;
	}
}

void send_filename() {
	int sockfd;
	int i;
	fileaddress_t *tmpfileaddrp;

	for (i = 0; i < userservcnt; i++) {
		tmpfileaddrp = &fileaddrsp[i];
		if (tmpfileaddrp->servavail == 0)
			continue;
		sockfd = fileaddrsp[i].sockfd;
		create_buffer(FILENAME, filename, buffer);
		Write(sockfd, buffer, MAXFILENAMESIZE);
		printf("sendfilename %s \n", filename);
	}
}

void get_file_status() {
	FILE *fp;
	int sockfd; 
	tlv_t *bufstp = NULL;
	fileaddress_t *tmpfileaddrp;
	int i = 0;
	
	for (i = 0; i < userservcnt; i++) {
		tmpfileaddrp = &fileaddrsp[i];
		if (tmpfileaddrp->servavail == 0)
			continue;
		sockfd	= tmpfileaddrp->sockfd;
		send_query_info(i);
		printf("get_file_status 1 \n");
		sleep(2);
		Read(sockfd, buffer, MAXBUFSIZE);
		printf("get_file_status 2 \n");

		retrieve_buffer(buffer, &bufstp);
		
		printf("getfilestatus: %ld \t %ld \n", bufstp->buf_cksize, bufstp->buf_offset);
		printf("\nerror %d\n", bufstp->buf_err); 
		if (bufstp->buf_err == 0) {
			fileinforx = 1;
			break;	 
		}
		else 
			printf(FILEINVALIDSTR);
			
	}
	filesize = bufstp->buf_cksize;
	filefd = Open(filename, O_CREAT|O_WRONLY|O_TRUNC, FILE_MODE);
	ftruncate(filefd, filesize); 
	printf("get_file_status \n");
	fp = fdopen(filefd, "w");
	fclose(fp);
	printf("get_file_status \n");
}

#define MAXTXSIZE 10
void get_file_data(int serverno) {
	FILE *fp;
	size_t count = 0;
	chunkinfo_t *infop = &(fileaddrsp[serverno].chunkinfo);
	long totalsize = infop->size;
	long seek = infop->off;
	int i = 0;
	char buf[MAXTXSIZE + 1];
	int sockfd = fileaddrsp[serverno].sockfd;
	
	printf("getfiledata %d %ld %ld \n", serverno, infop->size, infop->off);

    	
	filefd = Open(filename, O_CREAT|O_WRONLY, FILE_MODE);
		
	fp = fdopen(filefd, "w");

	pthread_mutex_lock(&lock);
	fseek(fp, seek, SEEK_SET);

	do {
		if (totalsize - MAXTXSIZE < 0)
			count = totalsize;
		else
			count = MAXTXSIZE;
		Read(sockfd, buf, count); 	
		Write(filefd, buf, count);
		pthread_mutex_unlock(&lock);
		totalsize -= MAXTXSIZE;
		i++;
	} while(totalsize >= 0);	
	fclose(fp);
}
