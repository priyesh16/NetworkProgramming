#include <stdio.h>
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

#define MAXPORTNO 65535
#define SUCCESS 0
#define BASE 10

int isipaddr(char *addr); 

int isport(unsigned long port);

int retrieveport(const char *portstr, unsigned long *portno);

void usage(char *usage); 
