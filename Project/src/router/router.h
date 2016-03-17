#ifndef ROUTER_H
#define ROUTER_H

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

#define MAXADDRSIZE 50
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
#define USAGE "\n ./router <Node Name (A, B, C ...)> <port no (1-65535)> \n"
#define BACKLOG 1
#define BASE 10
#define MAXERRLEN 100
#define MAXLINELEN 100
#define NBR_CONF_FILE "neighbor.config"
#define NODE_CONF_FILE "node.config"
#define MAXNBRS 15

void start_log(char *filename);
void mallocgp();
void freegp();

void send_file_status();
int read_packet(int connno);

int parse_packet(int conn);
int get_ip(int conn);
void create_socket();
int get_listen_socket(int sockfd);

void send_file_data();
int get_hostname(int conn);
int validate_hostname(int conn);
int validate_type(int conn); 
void print_loginfo(int conn , int flag);
void start_log(char *filename); 

typedef struct nbr_s {
	char	nbr_name;
	int		nbr_cost;
	struct sockaddr_in	sockaddr;
}nbr_t;

#define nbrfam sockaddr.sin_family
#define nbraddr sockaddr.sin_addr
#define nbrport sockaddr.sin_port


typedef struct node_s {
	char	node_name;
	int		node_cost;
	nbr_t	node_nbr[MAXNBRS];
	int		node_nbr_cnt;
	int		node_sockfd;
	unsigned short node_port;
} node_t;

node_t selfnode;

#define mynbrcnt selfnode.node_nbr_cnt
#define myname	selfnode.node_name
#define mysock	selfnode.node_sockfd
#define myport	selfnode.node_port
#define mynbr selfnode.node_nbr

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
	int		servsockfd;
	int		clisockfd;
	short	cliport;
	int		type;
	int		version;
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

pthread_mutex_t lock;

void myprintf(int flag, const char *format, ...); 
void myfree(void *ptr);
void retrieve_port(const char* portstr, unsigned short *portno);
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


int connect_client(int conn);
void Pthread_create(pthread_t *tid, const pthread_attr_t *attr,
               void * (*func)(void *), void *arg);
void Pthread_join(pthread_t tid, void **status);
#endif
