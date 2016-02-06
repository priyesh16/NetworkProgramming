#include <common.h>

#define INVALIDFILE "invalid file"
/* check if ip address is valid */
int isipaddr(char *addr) {
	struct sockaddr_in sa;
	return inet_pton(AF_INET, addr, &(sa.sin_addr));
}

/* check if the port number is valid */
int isport(unsigned long port) {
	int err = SUCCESS;

	if (port <= 0 || port >= MAXPORTNO){
		printf("port %lu out of range", port); 
		return ERANGE;
	}
	
	return err; 
}

/* given a string input of port number convert to an unsigned long */
void retrieveport(const char* portstr, unsigned long *portno){
	unsigned long port;
	char *endptr;
	int err = SUCCESS;

	port = strtoul(portstr, &endptr, BASE);
	if((err = isport(port)) != 0)
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
void sendack(void) {
	fflush(NULL);
	printf("%s", ACK);
	fflush(NULL);
}

/* validate the number of arguments and print useage
 * if invalid
 */
void validatearg(int argc, int max, char *useage){
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

int retrievebuffer(char *buffer, tlv_t **buffstpp) {
	tlv_t *buffstp;
	char *writer = buffer;
	
	buffstp = (tlv_t *)malloc(sizeof(tlv_t));
	if (buffstp == NULL)
		return MEMERR;
	bzero(buffstp, sizeof(tlv_t)); 
		
	buffstp->buf_chunk = (char *)malloc(MAXBUFSIZE * sizeof(char));
	if (buffstp->buf_chunk == NULL)
		return MEMERR;
	bzero(buffstp->buf_chunk, MAXBUFSIZE * sizeof(char)); 
	
	buffstp->buf_type = atoi(writer);
	writer += sizeof(type_t);
	buffstp->buf_len = atoi(writer);
	writer += sizeof(size_t);
	buffstp->buf_err = atoi(writer);
	writer += sizeof(int);
	
	switch (buffstp->buf_type) {
		case FILESIZE:
			buffstp->buf_fsize = atol(writer);
			printf("\n%d \t%d \t%d \t%lu \n", buffstp->buf_type, buffstp->buf_len, 
				buffstp->buf_err, buffstp->buf_fsize);
			break;
		case FILECHUNK:
			strcpy(buffstp->buf_chunk, writer);
				printf("\n%d \t%d \t%d \t%s \n", buffstp->buf_type, buffstp->buf_len, 
				buffstp->buf_err, buffstp->buf_chunk);
			break;
		case FILEERROR:
			strcpy(buffstp->buf_chunk, writer);
			printf("\n%d \t%d \t%d %s\t \n", buffstp->buf_type, buffstp->buf_len, 
				buffstp->buf_err, buffstp->buf_error);
			break;
	}

	*buffstpp = buffstp;
	return SUCCESS;
}


void createbuffer(type_t type, void *val, char *buffer) {
	char *writer = buffer;
	sprintf(writer, "%d", type);
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
			writer += sizeof(ssize_t);
			sprintf(writer	, "%d", *(int *)val);
			writer += sizeof(int);
			sprintf(writer, "%s", INVALIDFILE);
			break;
	}
}


	

