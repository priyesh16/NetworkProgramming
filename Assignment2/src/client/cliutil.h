#ifndef CLIUTIL_H
#define CLIUTIL_H

#include <common.h>

#define MAXLINES 10
#define MAXLINELEN 15 
#define NEWLINE '\n'
#define LOCALHOST "127.0.0.1"
#define LOCALHOSTSTR "localhost"
/* */

typedef struct fileaddress_s {
	struct	sockaddr_in addr;
	int 	available;     //flag to check if the server is available
}fileaddress_t;


int userservcnt;
int availservcnt;
int fileservcnt;
fileaddress_t *fileaddrsp;

int getipaddressfromfile(struct sockaddr_in serv_addrs[]); 


/* return a listen socket bound on  given port */
int getudpsocket(unsigned long port); 

int getipaddrfromfile(const char *filename); 
	   
#endif
