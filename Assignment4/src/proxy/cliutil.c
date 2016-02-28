#include "client.h"

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
	int connfd = 0;
	connfd = Accept(sockfd, (struct sockaddr*)NULL, NULL);

	/* close listening socket and return the client socket */
	return connfd;
}

void start_log(char *filename) {
	if (!strcmp(filename, "stdout"))
		fp = stdout;
	else			
		fp = fopen(filename, "a+");
}

int read_packet(int conn) {
	int connfd = curconn.servsockfd;
	int n = 10;
	fd_set readfd;
	struct timeval tv;
	int maxfd = connfd + 1;
	
	FD_ZERO(&readfd);
	FD_SET(connfd, &readfd);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	Select(maxfd, &readfd, NULL, NULL, &tv);
	printf("Reading request ...\n");
	
	if (FD_ISSET(connfd, &readfd))
		n = read(connfd, curconn.request, MAXBUFSIZE);
	if (n <= 0) {
		printf("Read from client failed\n");
		curconn.error = CONN_CLOSED;
		return ERROR;
	}
	if (!parse_packet(conn))
		return ERROR;
	if (!send_request(conn))
		return ERROR;
	if (!send_response(conn))
		return ERROR;
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


