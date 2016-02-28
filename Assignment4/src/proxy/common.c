#include "common.h"

errortable_t errortab[] = {
	{BAD_REQUEST, "Bad Request\r\n\r\n"},
	{FORBIDDEN, "Forbidden\r\n\r\n"},
	{NOT_FOUND, "Host not found \r\n\r\n"},
	{METHOD_NOT_ALLOWED, "Method Not Allowed\r\n\r\n"},
	{NOT_IMPLEMENTED, "Not Implemented\r\n\r\n"},
	{CONN_CLOSED, "Connection Closed\r\n\r\n"},
	{-1, ""},

};

void send_error(int conn) {
	char header[MAXBUFSIZE];
	char *writer = header; 
	int i;
	int sockfd = curconn.servsockfd;

	sprintf(header, "%s", HTTPHEADER);
	writer = header + strlen(header) - 1;
	for (i = 0; errortab[i].err != -1; i++) {
		if (errortab[i].err == curconn.error) {	
			sprintf(writer, " %d ", errortab[i].err);
			strcat(writer, errortab[i].errstr);
		}
	}
	Write(sockfd, header, strlen(header)); 
	printf("Sending error \n%s", header);
}

void myprintf(int flag, const char *format, ...) {
	va_list args;
	time_t secs = time(NULL);
	struct tm *parse = localtime(&secs);
	char str[100];
	
	if (flag) {
		strftime(str, 100, "%A, %B %d %Y%l:%M %p", parse);
		fprintf(fp, "%s: ", str);
	}
	va_start (args, format);
	vfprintf(fp, format, args);
	va_end(args);
	fflush(NULL);
}


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
void retrieve_port(const char* portstr, short *portno){
	short port;
	char *endptr;
	int err = SUCCESS;

	port = strtoul(portstr, &endptr, BASE);
	if((err = is_port(port)) != SUCCESS)
		err_sys("%s \n ", "Invalid Port");
	//if (*endptr != '\0')
	//	printf("Making use of port %lu from %s \n", port, portstr);
	*portno = port;
}

/* print usage */
void usage(char *usage) {
	printf("usage is %s \n", usage);  
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


void myfree(void *ptr) {
	if (ptr != NULL)
		free(ptr);
}

