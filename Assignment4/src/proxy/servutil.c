#include "server.h"

void mallocgp() {
	buffer = (char *)malloc(MAXBUFSIZE * sizeof(char));
	destaddrgp = (struct sockaddr *)malloc(sizeof(struct sockaddr));
	infogp = (chunkinfo_t *)malloc(sizeof(chunkinfo_t));
	if (buffer == NULL || destaddrgp == NULL || infogp == NULL) {
		printf("no space, quitting ... \n");
		exit(-1);
	}
}

void freegp() {
	if (buffer != NULL)
		free(buffer);
	if (destaddrgp != NULL)
		free(destaddrgp);
	if (infogp != NULL)
		free(infogp);
}

void myprintf(const char *format, ...) {
	va_list args;
	time_t secs;

	secs = time(NULL);

	printf("%s : \n", ctime(&secs));
	va_start (args, format);
	vprintf(format, args);
	va_end(args);
	fflush(NULL);
}

/* return a listen socket bound on  given port */
int create_socket(unsigned long port) {
	int sockfd = 0;
	struct sockaddr_in serv_addr; 

	/* fill socket data structure given a port */
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port); 

	/* listen to client and return an accepted socket */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	Bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	Listen(sockfd, BACKLOG);
	return sockfd;
}

void get_chunk_info() {
	tlv_t *bufstp;
	
	printf("getting chunk info ... \n");		

	retrieve_buffer(buffer, &bufstp);
	infogp->size = bufstp->buf_cksize;
	infogp->off  = bufstp->buf_offset;
	send_file_data();
	myfree(bufstp);
}

/* This function mallocs a tlv_t structure, freeing
 * of the structure should be taken care by the caller
 */
void serv_free_all() {

}

int get_listen_socket(int sockfd) {
	int connfd = 0;
	connfd = Accept(sockfd, (struct sockaddr*)NULL, NULL);

	/* close listening socket and return the client socket */
	return connfd;
}

void get_filename() {
	tlv_t *buffstp;

	retrieve_buffer(buffer, &buffstp);
	strcpy(filename, buffstp->buf_fname);
	myfree(buffstp);
	//printf("get_filename %s \n", filename);
}


int read_packet(int conn) {
	int connfd = curconn.servsockfd;
	type_t type = 0;
	int n = 10;
	fd_set readfd;
	struct timeval tv;
	int maxfd = connfd + 1;
	
	FD_ZERO(&readfd);
	FD_SET(connfd, &readfd);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	Select(maxfd, &readfd, NULL, NULL, &tv);
	printf("Reading packet ...\n");
	
	if (FD_ISSET(connfd, &readfd))
		n = read(connfd, curconn.request, MAXBUFSIZE);
	if (n <= 0) {
		printf("Error reading \n");
		return ERROR;
	}
	printf("Read packet \n %s ...", curconn.request);
	if (!parse_packet(conn))
		return ERROR;
	else if (!send_request(conn));
		return ERROR;
	return type;
}

int validate_type(int conn) {
	curconn.type = -1;

	if (strstr(curconn.request, "HEAD")) 
		curconn.type = HEAD;
	if (strstr(curconn.request, "GET"))
		curconn.type = GET;
	if (curconn.type == -1) 
		curconn.error = METHOD_NOT_ALLOWED;

	return SUCCESS;
}

int validate_hostname(int conn) {
	char *hostname = curconn.hostname;
	int i = 0;
	char *banned[] = {	"www.angrybirds.com",
						"www.youtube.com" 
						};

	for (i = 0;  i <= 1; i++) {
		if (!strcmp(hostname, banned[i])) {
			curconn.error = FORBIDDEN;
			printf("hostname %s not allowed \n", hostname);
			return ERROR;
		}
	}
	return SUCCESS;
}

int get_hostname(int conn) {
	char *hostline;
	char *hostname;
	char *portstr;
	char *tmpstr;
	short port = 80;
	
	hostline = (char *)malloc(MAXBUFSIZE);
	if ((tmpstr = strstr(curconn.request, "Host"))) {
		printf("type : %d \n", curconn.type);
		strcpy(hostline, tmpstr);
		hostname = strtok(hostline, "\r\n"); 		
		//hostname = strtok(NULL, ":");
		
		hostname += strlen("Host: "); // Gets rid of Host:
		hostname = strtok(hostname, ":"); // Strips port if port exists
		portstr = strtok(NULL, " "); // Tries to get portstr
		if (portstr != NULL) {
			printf("port %s \n", portstr);
			retrieve_port(portstr, &port);
		}
		
		curconn.cliport = port;
		strcpy(curconn.hostname, hostname);

		if(!validate_hostname(conn)) {
			free(hostline);	
			return ERROR;
		}
	}
	free(hostline);
	return SUCCESS;
}

int parse_packet(int conn) {
	if (!validate_type(conn))
		return ERROR;

	if(!get_hostname(conn)) {
		return ERROR;
	}
	if(!get_ip(conn)) 
		return ERROR;

	return SUCCESS;
}
	

int get_ip(int conn) {
	char ip[100];
	struct hostent *he;
	struct	in_addr **cliaddrlist;
	int i;
	char *hostname = curconn.hostname;
	curconn.cliaddr = (struct in_addr *)malloc(sizeof(struct in_addr));
         
	if ( (he = gethostbyname(hostname)) == NULL) {
        	// get the host info
        	herror("gethostbyname");
        	return ERROR;
    }
 
	cliaddrlist = (struct in_addr **)he->h_addr_list;
     
	for(i = 0; cliaddrlist[i] != NULL; i++) {
		memcpy(curconn.cliaddr, cliaddrlist[i], sizeof(struct in_addr));
		//Return the first one;
		strcpy(ip , inet_ntoa(*curconn.cliaddr));
		printf("%s resolved to %s \n" , hostname , ip);
     		return SUCCESS;
	}

	return ERROR;
}



void send_file_status() {
	int err = SUCCESS;
	struct stat st = {0};

	//printf("send_file status %s\n", filename);
	err = stat(filename, &st);
	if (err != 0)
		create_buffer(FILEERROR, &errno, buffer);
	else
		create_buffer(FILESIZE, &(st.st_size), buffer);
	printf("sending file size: size %ld\n", st.st_size);
	//send_syn_ack(sockfd, buffer);
	Sendto(sockfd, buffer, MAXBUFSIZE, SENDFLAG, destaddrgp, SOCKADDRSZ);
}


void send_file_data() {
	FILE *fp;
	int fd;
	long seek = infogp->off;
	size_t count = 0;
	long totalsize = infogp->size;
	long leftover = totalsize;
	long start = seek;
	long end = seek;
	unsigned long i = 1;
	char data[MAXTXSIZE + 1];
	//char *writer;
	struct sockaddr_in *destaddr_inp; 
	char address[20];
	unsigned short port;
	
	printf("sending from offset %ld to %ld from file %s ...\n", seek, seek + totalsize, filename);
	fd = Open(filename, O_RDONLY, FILE_MODE);
	fp = fdopen(fd, "r");
	fseek(fp, seek, SEEK_SET);	

	do {
		//writer = buffer;
		bzero(data, MAXTXSIZE + 1);
		bzero(buffer, MAXBUFSIZE + 1);
		if (leftover - MAXTXSIZE < 0)
			count = leftover;
		else
			count = MAXTXSIZE;

		Read(fd, data, count);
		//printf("****************************\n");
		//printf("read data \n %s \n", data);
		//printf("****************************\n");
		
		start = end;
		if (count != MAXTXSIZE)
			end = seek + totalsize;
		else
			end = seek + (i * count);
		//printf("****************************\n");
		
		//printf("\n  %s \t", writer);
		//writer += sizeof(type_t);
		//printf("%s \t", writer);
		//writer += sizeof(size_t);
		//printf("%s \t", writer);
		//writer += sizeof(int);
		//printf("%s \t", writer);
		//writer += sizeof(unsigned long);
		//printf("%s \n", writer);
		//printf("****************************\n");
		destaddr_inp = (struct sockaddr_in *)destaddrgp;
		port = destaddr_inp->sin_port;
		inet_ntop(AF_INET, &(destaddr_inp->sin_addr), address, INET_ADDRSTRLEN);
		
		add_packet_ident(i, data);
		//if (globi != i) {
			Sendto(sockfd, buffer, PACKETSIZE, SENDFLAG, destaddrgp, SOCKADDRSZ);	
			printf("sent packet %ld from %ld to %ld to %s:%u...\n", i, start, end, address, port);
		//}		
		//else
		//	globi = 0;		
		leftover -= MAXTXSIZE;
		i++;
	} while(leftover > 0);	
}

void add_packet_ident(unsigned long i, char *data) {
	char *writer = buffer;

	sprintf(writer, "%d", FILECHUNK);
	writer += sizeof(type_t);
	sprintf(writer, "%lu", strlen((char *)data));
	writer += sizeof(size_t);
	sprintf(writer, "%d", NOERROR);
	writer += sizeof(int);
	sprintf(writer, "%lu", i);
	writer += sizeof(unsigned long);
	strcpy(writer, data);
}


