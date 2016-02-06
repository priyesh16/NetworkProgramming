#include <common.h>

#define INVALIDFILE "invalid file"
/* check if ip address is valid */
int is_ip_addr(char *addr) {
	struct sockaddr_in sa;
	return inet_pton(AF_INET, addr, &(sa.sin_addr));
}

/* check if the port number is valid */
int is_port(unsigned long port) {
	int err = SUCCESS;

	if (port <= 0 || port >= MAXPORTNO){
		printf("port %lu out of range", port); 
		return ERANGE;
	}
	
	return err; 
}

/* given a string input of port number convert to an unsigned long */
void retrieve_port(const char* portstr, unsigned long *portno){
	unsigned long port;
	char *endptr;
	int err = SUCCESS;

	port = strtoul(portstr, &endptr, BASE);
	if((err = is_port(port)) != 0)
		err_sys("%s \n ", "Invalid Port");
	if (*endptr != '\0')
		printf("Making use of port %lu from %s \n", port, portstr);
	*portno = port;
}

/* print usage */
void usage(char *usage) {
	printf("usage is %s \n", usage);  
}

/* send a termination ack so that the client knows when the 
 * message has ended. The caller dups stdout to socket so we
 * can print the ACK to stdout and it would get written to socket
 */
void send_ack(void) {
	fflush(NULL);
	printf("%s", ACK);
	fflush(NULL);
}

/* validate the number of arguments and print useage
 * if invalid
 */
void validate_arg(int argc, int max, char *useage){
	if (argc != max){
		usage(useage);
		err_sys("%s \n ", "Invalid No. of Arguments");
	}
}


/* This function mallocs a tlv_t structure, freeing
 * of the structure should be taken care by the caller
 */
void freeall() {

}

void myexit(const char *errstr) {
	printf("%s", errstr);
	freeall();
	fflush(NULL);
	exit(1);
}

int retrieve_buffer(char *buffer, tlv_t **buffstpp) {
	tlv_t *buffstp;
	char *writer = buffer;
	
	buffstp = (tlv_t *)malloc(sizeof(tlv_t));
	if (buffstp == NULL)
		return MEMERR;
	bzero(buffstp, sizeof(tlv_t)); 
		
	buffstp->buf_data = (char *)malloc(MAXBUFSIZE * sizeof(char));
	if (buffstp->buf_data == NULL)
		return MEMERR;
	bzero(buffstp->buf_data, MAXBUFSIZE * sizeof(char)); 
	printf("rb : %s \n", writer);	
	buffstp->buf_type = atoi(writer);
	writer += sizeof(type_t);
	printf("rb : %s \n", writer);	
	buffstp->buf_len = atoi(writer);
	writer += sizeof(size_t);
	printf("rb : %s \n", writer);	
	buffstp->buf_err = atoi(writer);
	writer += sizeof(int);
	
	switch (buffstp->buf_type) {
		case FILESIZE:
			buffstp->buf_fsize = atol(writer);
			printf("\n%d \t%ld \t%d \t%lu \n", buffstp->buf_type, buffstp->buf_len, 
				buffstp->buf_err, buffstp->buf_fsize);
			break;
		case FILECHUNK:
			strcpy(buffstp->buf_data, writer);
				printf("\n%d \t%ld \t%d \t%s \n", buffstp->buf_type, buffstp->buf_len, 
				buffstp->buf_err, buffstp->buf_data);
			break;
		case FILEERROR:
			printf("rb : %s \n", writer);	
			strcpy(buffstp->buf_error, writer);
			printf("\n%d \t%ld \t%d %s\t \n", buffstp->buf_type, buffstp->buf_len, 
				buffstp->buf_err, buffstp->buf_error);
			break;
	}

	*buffstpp = buffstp;
	return SUCCESS;
}

void get_chunk_info(int sockfd, char *buffer) {
}

void send_chunk_info(int sockfd, char *buffer, int size, int offset) {
	int err = SUCCESS;
	chunkinfo_t info = {0};	
	info.size = size;
	info.start_off = offset;
	create_buffer(FILEERROR, &errno, buffer);
	Write(sockfd, buffer, MAXBUFSIZE);
}

void send_file_status(int sockfd, char *buffer) {
	int err = SUCCESS;
	struct stat st = {0};
	
	Read(sockfd, buffer, MAXBUFSIZE * sizeof(char));
	
	err = stat(buffer, &st);
	if (err != 0)
		create_buffer(FILEERROR, &errno, buffer);
	else 
		create_buffer(FILESIZE, &(st.st_size), buffer); 
	Write(sockfd, buffer, MAXBUFSIZE);
}

void get_file_status(int sockfd) {
	char filename[MAXFILENAMESIZE];
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
		
		Write(sockfd, filename, MAXFILENAMESIZE);
		/* if user enters "exit" then quit, server on receiving
		 * the "exit" command will close the socket and open a
		 * new one for the next client.
		 */
		Read(sockfd, buffer, MAXBUFSIZE);
		retrieve_buffer(buffer, &bufstp);
		printf("\n%d \t%ld \t%d %s\t \n", bufstp->buf_type, bufstp->buf_len, 
				bufstp->buf_err, bufstp->buf_error);
		if (bufstp->buf_err == 0) 
			break;
		else {
			printf(FILEINVALIDSTR);
			continue;
		}	
	}
}


void create_buffer(type_t type, void *val, char *buffer) {
	char *writer = buffer;
	sprintf(writer, "%d", type);
	printf("cr : %s \n", writer);
	writer += sizeof(type_t);

	switch (type) {
		case FILESIZE:
			sprintf(writer, "%lu", sizeof(int));
			writer += sizeof(size_t);
			sprintf(writer	, "%d", 0);
			writer += sizeof(int);
			sprintf(writer, "%ld", *(off_t *)val);
			break;
		case FILECHUNK:
			sprintf(writer, "%lu", strlen((char *)val));
			writer += sizeof(size_t);
			sprintf(writer	, "%d", 0);
			writer += sizeof(int);
			sprintf(writer, "%s", (char *)val);
			break;
		case FILEERROR:
			sprintf(writer, "%lu", sizeof(int));
			printf("cr : %s \n", writer);
			writer += sizeof(ssize_t);
			sprintf(writer	, "%d", *(int *)val);
			printf("cr : %s \n", writer);
			writer += sizeof(int);
			sprintf(writer, "%s", INVALIDFILE);
			printf("cr : %s \n", writer);
			break;
	}
}


	

