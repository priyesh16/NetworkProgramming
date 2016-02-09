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

void myfree(void *ptr) {
	if (ptr != NULL)
		free(ptr);
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
		
	
	buffstp->buf_type = atoi(writer);
	writer += sizeof(type_t);
	buffstp->buf_len = atoi(writer);
	writer += sizeof(size_t);
	buffstp->buf_err = atoi(writer);
	writer += sizeof(int);
	
	switch (buffstp->buf_type) {
		case FILENAME:
			buffstp->buf_fname = (char *)malloc(MAXBUFSIZE * sizeof(char));
				if (buffstp->buf_fname == NULL)
				return MEMERR;
			bzero(buffstp->buf_fname, MAXBUFSIZE * sizeof(char)); 
			strcpy(buffstp->buf_fname, writer);
			//printf("\nrb : %d \t%ld \t%d %s\t \n", buffstp->buf_type, buffstp->buf_len, 
			//	buffstp->buf_err, buffstp->buf_fname);
			break;
		case FILESIZE:
			buffstp->buf_fsize = atol(writer);
			//printf("\nrb : %d \t%ld \t%d \t%lu \n", buffstp->buf_type, buffstp->buf_len, 
			//	buffstp->buf_err, buffstp->buf_fsize);
			break;
		case FILECHUNK:
			buffstp->buf_data = (char *)malloc(MAXBUFSIZE * sizeof(char));
			if (buffstp->buf_data == NULL)
				return MEMERR;
			bzero(buffstp->buf_data, MAXBUFSIZE * sizeof(char)); 
			strcpy(buffstp->buf_data, writer);
			//printf("\n rb %d \t%ld \t%d \t%s \n", buffstp->buf_type, buffstp->buf_len, 
			//	buffstp->buf_err, buffstp->buf_data);
			break;
		case FILEERROR:
			buffstp->buf_error = (char *)malloc(MAXBUFSIZE * sizeof(char));
			if (buffstp->buf_error == NULL)
				return MEMERR;
			bzero(buffstp->buf_error, MAXBUFSIZE * sizeof(char)); 
			strcpy(buffstp->buf_error, writer);
			//printf("\nrb : %d \t%ld \t%d %s\t \n", buffstp->buf_type, buffstp->buf_len, 
			//	buffstp->buf_err, buffstp->buf_error);
			break;
		case QUERYINFO:
			buffstp->buf_error = (char *)malloc(MAXBUFSIZE * sizeof(char));
			if (buffstp->buf_error == NULL)
				return MEMERR;
			bzero(buffstp->buf_error, MAXBUFSIZE * sizeof(char)); 

			strcpy(buffstp->buf_error, writer);
			//printf("\nrb : %d \t%ld \t%d %s\t \n", buffstp->buf_type, buffstp->buf_len, 
			//	buffstp->buf_err, buffstp->buf_error);
			break;		
		case CHUNKINFO:
			buffstp->buf_cksize = atol(writer);
			writer += sizeof(unsigned long);
			buffstp->buf_offset = atol(writer);
			//printf("\nrb : %d \t%ld \t%d \t%lu \t%lu \n", buffstp->buf_type, buffstp->buf_len, 
			//	buffstp->buf_err, buffstp->buf_offset, buffstp->buf_cksize);
			break;
		case SENDACK:
			buffstp->buf_error = (char *)malloc(MAXBUFSIZE * sizeof(char));
			if (buffstp->buf_error == NULL)
				return MEMERR;
			bzero(buffstp->buf_error, MAXBUFSIZE * sizeof(char)); 

			strcpy(buffstp->buf_error, writer);
			//printf("\nrb : %d \t%ld \t%d %s\t \n", buffstp->buf_type, buffstp->buf_len, 
			//	buffstp->buf_err, buffstp->buf_error);
			break;
	}

	*buffstpp = buffstp;
	return SUCCESS;
}


void create_buffer(type_t type, void *val, char *buffer) {
	char *writer = buffer;
	sprintf(writer, "%d", type);
	writer += sizeof(type_t);

	switch (type) {
		case FILENAME:
			sprintf(writer, "%lu", strlen((char *)val));
			writer += sizeof(size_t);
			sprintf(writer	, "%d", NOERROR);
			writer += sizeof(int);
			sprintf(writer, "%s", (char *)val);
			break;
		case FILESIZE:
			sprintf(writer, "%lu", sizeof(int));
			writer += sizeof(size_t);
			sprintf(writer	, "%d", NOERROR);
			writer += sizeof(int);
			sprintf(writer, "%ld", *(off_t *)val);
			break;
		case FILECHUNK:
			sprintf(writer, "%lu", strlen((char *)val));
			writer += sizeof(size_t);
			sprintf(writer	, "%d", NOERROR);
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
		case QUERYINFO:
			sprintf(writer, "%lu", sizeof(int));
			writer += sizeof(ssize_t);
			sprintf(writer	, "%d", *(int *)val);
			writer += sizeof(int);
			sprintf(writer, "%s", INVALIDFILE);
			break;
		case CHUNKINFO:
			sprintf(writer, "%lu", sizeof(chunkinfo_t));
			writer += sizeof(size_t);
			sprintf(writer, "%d", NOERROR);
			writer += sizeof(int);
			sprintf(writer, "%ld", ((chunkinfo_t *)val)->size);
			writer += sizeof(unsigned long);
			sprintf(writer, "%ld", ((chunkinfo_t *)val)->off);
			writer += sizeof(unsigned long);
			break;
		case SENDACK:
			sprintf(writer, "%lu", sizeof(int));
			writer += sizeof(ssize_t);
			sprintf(writer	, "%d", *(int *)val);
			writer += sizeof(int);
			sprintf(writer, "%s", INVALIDFILE);
			break;	
	}
	//printf("\n cb: %s\n", buffer);
}





	

