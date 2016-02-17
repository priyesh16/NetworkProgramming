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
	char *writer;
	struct sockaddr_in *destaddr_inp; 
	char address[20];
	short port;

	printf("sending from offset %ld to %ld from file %s ...\n", seek, seek + totalsize, filename);
	fd = Open(filename, O_RDONLY, FILE_MODE);
	fp = fdopen(fd, "r");
	fseek(fp, seek, SEEK_SET);	

	do {
		writer = buffer;
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
		
		add_packet_ident(i, data);
		start = end;
		if (count != MAXTXSIZE)
			end = seek + totalsize;
		else
			end = seek + (i * count);
		//printf("****************************\n");
		
		//printf("\n  %s \t", writer);
		writer += sizeof(type_t);
		//printf("%s \t", writer);
		writer += sizeof(size_t);
		//printf("%s \t", writer);
		writer += sizeof(int);
		//printf("%s \t", writer);
		writer += sizeof(unsigned long);
		//printf("%s \n", writer);
		//printf("****************************\n");
				
		Sendto(sockfd, buffer, PACKETSIZE, SENDFLAG, destaddrgp, SOCKADDRSZ);
		destaddr_inp = (struct sockaddr_in *)destaddrgp;
		port = destaddr_inp->sin_port;
		inet_ntop(AF_INET, &(destaddr_inp->sin_addr), address, INET_ADDRSTRLEN);
		printf("sent packet %ld from %ld to %ld to %s:%u...\n", i, start, end, address, port);
		leftover -= MAXTXSIZE;
		i++;
	} while(leftover >= 0);	
	printf("****************************\n");
}


/* return a listen socket bound on  given port */
void create_socket(unsigned long port) {
	struct sockaddr_in serv_addr; 

	/* fill socket data structure given a port */
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port); 

	/* listen to client and return an accepted socket */
	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	Bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
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

void get_filename() {
	tlv_t *buffstp;

	retrieve_buffer(buffer, &buffstp);
	strcpy(filename, buffstp->buf_fname);
	myfree(buffstp);
	//printf("get_filename %s \n", filename);
}

type_t get_type() {
	tlv_t *buffstp;
	type_t type;
	int n = 0;
	struct sockaddr dst_addr = {0};
	socklen_t size = sizeof(dst_addr);
	fd_set readfd;
	struct timeval tv;
	int maxfd = sockfd + 1;
	char address[20];
	int port;
	struct sockaddr_in *destaddr_inp;

	FD_ZERO(&readfd);
	FD_SET(sockfd, &readfd);
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	Select(maxfd, &readfd, NULL, NULL, &tv);

	if (FD_ISSET(sockfd, &readfd))
		n = recvfrom(sockfd, buffer, MAXBUFSIZE, RECVFLAG, &dst_addr, &size);
	if (n <= 0) 
		return CLIENTCLOSE;
	memcpy (destaddrgp, &dst_addr, sizeof(struct sockaddr));
	destaddr_inp = (struct sockaddr_in *)destaddrgp;
	port = destaddr_inp->sin_port;
	inet_ntop(AF_INET, &(destaddr_inp->sin_addr), address, INET_ADDRSTRLEN);

	retrieve_buffer(buffer, &buffstp);
	type = buffstp->buf_type;
	printf("got type %d from address %s:%d...\n", buffstp->buf_type, address, port);
	myfree(buffstp);
	return type;
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

