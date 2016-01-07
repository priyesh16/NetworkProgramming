#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>

#define MAXPORTNO 65535
#define SUCCESS 0
#define BASE 10

int isipaddr(char *addr); 

int isport(unsigned long port);

int retrieveport(const char *portstr, unsigned long *portno);

void usage(char *usage); 
