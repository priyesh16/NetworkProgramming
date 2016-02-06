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
	int 	servavail;     //flag to check if the server is available.
	int 	fileavail;     //flag to check if the server has the file to serve.
	int	sockfd;
}fileaddress_t;


int userservcnt; // Number of servers the user wants the chunks from.
int fileservcnt; // Number of servers in the file.
int availservcnt; // Number of available servers.
fileaddress_t *fileaddrsp; // Pointer to list of servers parsed out from the file.

int getipaddressfromfile(struct sockaddr_in serv_addrs[]); 


/* return a listen socket bound on  given port */
int getudpsocket(unsigned long port); 

int getipaddrfromfile(const char *filename); 
	   
#endif
