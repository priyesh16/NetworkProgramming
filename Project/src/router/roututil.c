#include "router.h"

#define NODECONFIGFILE "node.config"
#define MAXLINESIZE	30 

void readline(FILE *fp, char *line) {
	char ch = getc(fp);
	int count = 0;

	while ((ch != '\n') && (ch != EOF)) {
		line[count] = ch;
		count++;

		ch = getc(file);
	}

	line[count] = '\0';
}

int read_node_config() {
	FILE *fp;
	char line[MAXLINESIZE];

	fp = fopen(NODECONFIG, "r"); 
	if (!fp) {
		perror("read_node_config");
		return errno; 
	}

	while(!feof(fp)) {
		line = readline(fp);
		printf("%s", line);
	}
}

int send_request(int conn) {
int sockfd;
char *request;
int n;

connect_client(conn);
sockfd = curconn.clisockfd;
request = curconn.request;

/* Send request to the server, if server
* closes the connection, handle it.
*/
n = write(sockfd, request, strlen(request));
if (n <= 0) {
printf("Write to server failed \n");
curconn.error = CONN_CLOSED;		
return ERROR;
}
printf("Request \n %s \n", curconn.request);

/* Read response from the server, if server
* closes the connection, handle it.
*/
n = read(sockfd, curconn.response, MAXBUFSIZE);
if (n <= 0) {
printf("Read from client failed \n");
curconn.error = CONN_CLOSED;		
return ERROR;
}
fflush(NULL);
return SUCCESS;
}

int connect_client(int conn) {
int err = SUCCESS;
char address[MAXADDRSIZE];
struct	sockaddr_in addr;
struct sockaddr *castaddrp;
struct sockaddr_in myaddr;
int sockfd;

sockfd = Socket(AF_INET, SOCK_STREAM, 0);
curconn.clisockfd = sockfd;
memset((char *)&myaddr, 0, sizeof(myaddr));
myaddr.sin_family = AF_INET;
myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
myaddr.sin_port = htons(0);

	Bind(sockfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr));

	addr.sin_family = AF_INET;
	memcpy(&(addr.sin_addr), curconn.cliaddr, sizeof(struct sockaddr));
	addr.sin_port = htons(curconn.cliport);
	printf("Connecting to client...(connect might take 1 sec) \n");
	
	castaddrp = (struct sockaddr *)&addr;
	err = connect_timeo(sockfd, castaddrp, sizeof(struct sockaddr), 1);
	inet_ntop(AF_INET, &(addr.sin_addr), address, INET_ADDRSTRLEN);
	if (err < 0) { 		
		printf("Error : Could not connect to address:port %s:%d \n", address, ntohs(addr.sin_port));
		curconn.error = CONN_CLOSED;
		return ERROR;	
	}
	printf("Success : Connected to address:port %s:%d \n", address, ntohs(addr.sin_port));
	return SUCCESS;	
}

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

	pthread_mutex_lock(&lock);
	if (flag) {
		strftime(str, 100, "%A, %B %d %Y%l:%M %p", parse);
		fprintf(fp, "%s: ", str);
	}
	va_start (args, format);
	vfprintf(fp, format, args);
	va_end(args);
	fflush(NULL);
	pthread_mutex_unlock(&lock);
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

/* return a listen socket bound on  given port */
int create_socket(unsigned short port) {
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

int get_listen_socket(int sockfd) {
	int conn = 0;
	conn = accept(sockfd, (struct sockaddr*)NULL, NULL);
	if (conn == -1) {
		perror("Accept Error:");
		curconn.error = CONN_CLOSED;
		return ERROR;
	}

	/* close listening socket and return the client socket */
	return conn;
}

void start_log(char *filename) {
	if (!strcmp(filename, "stdout"))
		fp = stdout;
	else			
		fp = fopen(filename, "a+");
}

int read_packet(int conn) {
	int connfd = curconn.servsockfd;
	int n = 0;
	
	while(1) {
		printf("Reading request ...\n");
		n = read(connfd, curconn.request, MAXBUFSIZE);
		if (n < 0) {
			printf("Read from client failed\n");
			curconn.error = CONN_CLOSED;
			return ERROR;
		}
		if (n == 0) {
			printf("Read from client completed\n");
			return SUCCESS;
		}
		if (!parse_packet(conn))
			return ERROR;
		if (!send_request(conn))
			return ERROR;
		if (!send_response(conn))
			return ERROR;
	}
	return SUCCESS;
}

int send_response(int conn) {
	int n;

	n = write(curconn.servsockfd, curconn.response, strlen(curconn.response));
	if (n <= 0) {
		printf("Write to client faled \n");
		curconn.error = CONN_CLOSED;
		return ERROR;
	}
	printf("Response \n %s \n", curconn.response);
	fflush(NULL);
	return SUCCESS;
}

int validate_type(int conn) {
	curconn.type = -1;

	if (strstr(curconn.request, "HEAD")) 
		curconn.type = HEAD;
	if (strstr(curconn.request, "GET"))
		curconn.type = GET;
	if (curconn.type == -1) {
		print_loginfo(conn, 0);	
		myprintf(0, "Error: Method other than HEAD or GET not allowed\n");
		myprintf(0, "Action Taken: Filtered - Method Not Allowed error Sent to Host\n");
		curconn.error = METHOD_NOT_ALLOWED;
		return ERROR;
	}

	return SUCCESS;
}

int validate_hostname(int conn) {
	char *hostname = curconn.hostname;
	char line[MAXLINELEN];
	int banfd;

	if (!(banfd = open(FORBIDFILE, O_RDONLY, 0))) {
		printf("Error : Unable to open %s", FORBIDFILE);
		exit(ERROR);
	}

	while (Readline(banfd, line, MAXLINELEN)) {
		if (strstr(line, hostname)) {
			curconn.error = FORBIDDEN;
			print_loginfo(conn, 0);	
			myprintf(0, "Error: Hostname %s not allowed \n", hostname);
			myprintf(0, "Action Taken: Filtered - Forbidden error Sent to Host\n");
			return ERROR;
		}
	}
	return SUCCESS;
}

int validate_header(int conn) {
	char *header;
	char *http1;
	char *http0;
	char *request;
	
	request = (char *)malloc(MAXBUFSIZE);
	strcpy(request, curconn.request);
	header = strtok(request, "\r\n"); 		
	http1 = strstr(header, "HTTP/1.1");
	http0 = strstr(header, "HTTP/1.0");
	if (!(http1 || http0)) {
		curconn.error = BAD_REQUEST;
		print_loginfo(conn, 0);	
		myprintf(0, "Error:  Header not HTTP1.1 or HTTP 1.0 \n");
		myprintf(0, "Action Taken: Filtered - Bad Request error Sent to Host\n");
		return ERROR;
	}
	if (http1)
		curconn.version = 1;
	else
		curconn.version = 0;

	free(request);
	return SUCCESS;
}

void print_loginfo(int conn , int flag) {
	char *hostline;
	char *tmpstr;
	char *hostname;	
	char *request = curconn.request;

	hostline = (char *)malloc(MAXBUFSIZE);
	strcpy(hostline, request);
	if ((tmpstr = strstr(hostline, "User-Agent"))) {
		hostname = strtok(tmpstr, "\r\n"); 		
		hostname += strlen("User Agent: "); 
	}

	myprintf(1, "\n");
	if (curconn.type == GET) 
		myprintf(0, "Type : GET \n"); 
	if (curconn.type == HEAD) 
		myprintf(0, "Type : HEAD \n"); 

	myprintf(0, "Requesting Host : %s\n", hostname);
	myprintf(0, "URI : %s\n", curconn.uri);
	if (flag) {
		myprintf(0, "Server Address : %s\n", inet_ntoa(*curconn.cliaddr));
		myprintf(0, "Action Taken : Forwarded\n");
		myprintf(0, "Error:  No Error \n");
	}

	free(hostline);
	return;
}

int get_hostname(int conn) {
	char *hostline;
	char *hostname;
	char *portstr;
	char *tmpstr;
	short port = 80;
	
	hostline = (char *)malloc(MAXBUFSIZE);
	if ((tmpstr = strstr(curconn.request, "Host"))) {
		strcpy(hostline, tmpstr);
		hostname = strtok(hostline, "\r\n"); 		
		//hostname = strtok(NULL, ":");
		
		hostname += strlen("Host: "); // Gets rid of Host:
		strcpy(curconn.uri, hostname);
		hostname = strtok(hostname, ":"); // Strips port if port exists
		portstr = strtok(NULL, " "); // Tries to get portstr
		if (portstr != NULL) {
			//myprintf("port %s \n", portstr);
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
	if (!validate_header(conn))
		return ERROR;

	if (!validate_type(conn))
		return ERROR;

	if(!get_hostname(conn)) {
		return ERROR;
	}

	if(!get_ip(conn)) 
		return ERROR;

	print_loginfo(conn, 1);	

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
			printf("Could not resolve %s \n", hostname);
        	herror("gethostbyname");
			curconn.error = NOT_FOUND; 
        	return ERROR;
    }
 
	cliaddrlist = (struct in_addr **)he->h_addr_list;
     
	for(i = 0; cliaddrlist[i] != NULL; i++) {
		memcpy(curconn.cliaddr, cliaddrlist[i], sizeof(struct in_addr));
		//Return the first one;
		strcpy(ip , inet_ntoa(*curconn.cliaddr));
		//myprintf("%s resolved to %s \n" , hostname , ip);
     		return SUCCESS;
	}

	return ERROR;
}


