#ifndef CLIUTIL_H
#define CLIUTIL_H

#include <common.h>

#define MAXLINES 10
#define MAXLINELEN 15 
#define NEWLINE '\n'
#define LOCALHOST "127.0.0.1"
#define LOCALHOSTSTR "localhost"
#define FILEINVALIDSTR "\n> Err: Invalid File, try again\n"
/* */

typedef struct fileaddress_s {
	struct	sockaddr_in addr;
	chunkinfo_t chunkinfo;
	int 	servavail;     //flag to check if the server is available.
	int 	fileavail;     //flag to check if the server has the file to serve.
	int	sockfd;
	pthread_t thread;
}fileaddress_t;
#define cksize chunkinfo.size 
#define ckoff chunkinfo.off  

long firstchunksize;
long otherchunksize;
int userservcnt; // Number of servers the user wants the chunks from.
int fileservcnt; // Number of servers in the file.
int availservcnt; // Number of available servers.
fileaddress_t *fileaddrsp; // Pointer to list of servers parsed out from the file.
char *buffer;
int filefd;
long int filesize;
int globserverno;
int fileinforx;
char filename[MAXFILENAMESIZE];
int getipaddressfromfile(struct sockaddr_in serv_addrs[]); 
pthread_mutex_t lock;

/* return a listen socket bound on  given port */
int getudpsocket(unsigned long port); 
void send_chunk_info(int serverno);	
int getipaddrfromfile(const char *filename); 
void get_file_data(int serverno);
void calculate_chunk_size();
void *thread_func(void *serverno);
void get_file_status();
void send_filename();
void get_file_from_user();

void get_ack(int sockfd, char *buffer);
int get_ipaddr_list(const char *filename);
void check_serv_avail();
void Pthread_create(pthread_t *tid, const pthread_attr_t *attr,
               void * (*func)(void *), void *arg);
void Pthread_join(pthread_t tid, void **status);
#endif
