#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <poll.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../head/unp.h"

#define HTTPHEADER "HTTP/1.1 "
#define MAXPORTNO 65535
#define SUCCESS 1
#define ERROR	0		
#define K	1024
#define MAXBUFSIZE (1024 * K)
#define MAXFILENAMESIZE 50
#define MAXHOSTNAMESIZE 50
#define MEMERRSTR "Err: Heap memory full..exiting"
#define SOCKADDRSZ sizeof(struct sockaddr)
#define MAXCONNECTIONS 100
#define curconn	connection[conn] 
#define USAGE "\n ./proxy <port no (1-65535)> <log filename (stdout for printing to console)> \n"
#define BACKLOG 1
#define BASE 10
#define MAXERRLEN 100
#define MAXLINELEN 100
#define FORBIDFILE	"forbidden-sites"

typedef enum error_s {
	MEMERR,
	BAD_REQUEST = 400,
	NOT_FOUND = 404,
	FORBIDDEN = 403,
	METHOD_NOT_ALLOWED = 405,
	CONN_CLOSED = 500,
	NOT_IMPLEMENTED = 501,
}error_t;

typedef struct conn_s {
	int	servsockfd;
	int	clisockfd;
	short	cliport;
	int	type;
	int	version;
	char	uri[MAXHOSTNAMESIZE];
	char	hostname[MAXHOSTNAMESIZE];
	struct	in_addr *cliaddr;
	error_t	error;
	char 	request[MAXBUFSIZE];
	char 	response[MAXBUFSIZE];
	pthread_t thread;
}conn_t;

conn_t connection[MAXCONNECTIONS];
FILE *fp;


typedef struct errortable_s {
	error_t err;
	char errstr[MAXERRLEN];
}errortable_t; 

typedef enum type_s {
	HEAD,
	GET,
}type_t;

void myprintf(int flag, const char *format, ...); 
void myfree(void *ptr);
void retrieve_port(const char* portstr, short *portno);
void Pthread_create(pthread_t *tid, const pthread_attr_t *attr,
		               void * (*func)(void *), void *arg);
void Pthread_join(pthread_t tid, void **status);
void *thread_func(void *serverno);

/* validate the number of arguments and print useage 
 *  * if invalid 
 *   */
void validate_arg(int argc, int max, char *useage);
	 
/* check if ip address is valid */
int isipaddr(char *addr); 

/* check if the port number is valid */
int isport(unsigned long port); 
int send_response(int conn);

void freeall();
void myexit(const char *errstr);
int  send_request(int conn);
void send_error(int conn);
#endif
