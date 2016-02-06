#ifndef CLIUTIL_H
#define CLIUTIL_H

#include <common.h>

#define MAXLINES 10
#define MAXLINELEN 15 
#define NEWLINE '\n'
#define LOCALHOST "127.0.0.1"
#define LOCALHOSTSTR "localhost"
/* */

int getipaddressfromfile(struct sockaddr_in serv_addrs[]); 


/* return a listen socket bound on  given port */
int getudpsocket(unsigned long port); 

int getipaddrfromfile(struct sockaddr serv_addrs[],
		        const char *filename, unsigned int *servercnt); 
	   
#endif
