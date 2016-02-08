#include "server.h"


void send_file_data(int sockfd, chunkinfo_t *infop) {
	FILE *fp;
	int fd;
	long seek = infop->off;
	size_t count = 0;
	long totalsize = infop->size;
	int i = 0;
	char buf[MAXTXSIZE + 1];
	
	fd = Open(filename, O_RDONLY, FILE_MODE);
	fp = fdopen(fd, "r");
	fseek(fp, seek, SEEK_SET);	

	do {
		if (totalsize - MAXTXSIZE < 0)
			count = totalsize;
		else
			count = MAXTXSIZE;
		Read(fd, buf, count); 	
		Write(sockfd, buf, count);
		totalsize -= MAXTXSIZE;
		i++;
	} while(totalsize >= 0);	
}

void send_syn_ack(int sockfd, char *buffer) {
	char ackstr[] = "ack";
	
	//printf("send_ack sockfd \n");

	create_buffer(SENDACK, ackstr , buffer);
	Write(sockfd, buffer, MAXBUFSIZE);
}


/* return a listen socket bound on  given port */
int get_listen_socket(unsigned long port) {
	int sockfd = 0, connfd = 0;
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
	connfd = Accept(sockfd, (struct sockaddr*)NULL, NULL);

	/* close listening socket and return the client socket */
	close(sockfd);
	return connfd;
}

void get_chunk_info(int sockfd, char *buffer, chunkinfo_t *infop) {
	tlv_t *bufstp;
	
	retrieve_buffer(buffer, &bufstp);
	infop->size = bufstp->buf_cksize;
	infop->off  = bufstp->buf_offset;
	send_file_data(sockfd, infop);
}


/* This function mallocs a tlv_t structure, freeing
 * of the structure should be taken care by the caller
 */
void serv_free_all() {
}

void get_filename(int sockfd, char *buffer) {
	tlv_t *buffstp;

	retrieve_buffer(buffer, &buffstp);
	strcpy(filename, buffstp->buf_fname);
	//printf("get_filename %s \n", filename);
}

type_t get_type(int sockfd, char *buffer) {
	tlv_t *buffstp;

	Read(sockfd, buffer, MAXBUFSIZE * sizeof(char));
	retrieve_buffer(buffer, &buffstp);
	//printf("get_type %d \n", buffstp->buf_type);
	return buffstp->buf_type;
}

void send_file_status(int sockfd, char *buffer) {
	int err = SUCCESS;
	struct stat st = {0};
	//printf("send_file status %s\n", filename);
	err = stat(filename, &st);
	if (err != 0)
		create_buffer(FILEERROR, &errno, buffer);
	else
		create_buffer(FILESIZE, &(st.st_size), buffer); 
	//send_syn_ack(sockfd, buffer);
	Write(sockfd, buffer, MAXBUFSIZE);
}



