#ifndef COMMON_H
#define COMMON_H

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
#include <../../head/unp.h>

#define MAXPORTNO 65535
#define SUCCESS 0
#define BASE 10
#define PROMPT ">:$"
#define ACK "ack" 
#define MAXDIGITS 5
#define NOOUT "No Output \n"
#define CMDLEN 100
#define CHUNKINFOLEN 100 
#define EXITCMD "exit"
#define MAXLINES 10
#define MAXLINELEN 35 
#define OUTLEN 332
#define MAXBUFSIZE 1000
#define MAXADDRSIZE 15
#define MAXFILENAMESIZE 50
#define MEMERRSTR "Err: Heap memory full..exiting"
#define NOERROR 0
#define NEWLINE '\n'
#define LOCALHOST "127.0.0.1"
#define LOCALHOSTSTR "localhost"
#define FILEINVALIDSTR "\n> Err: Invalid File, try again\n"
#define SENDFLAG    0
#define RECVFLAG    0
#define SOCKADDRSZ sizeof(struct sockaddr)
#define MAXTXSIZE 60
#define HEADERLEN (sizeof(type_t) + sizeof(size_t) + sizeof(int) + sizeof(unsigned long))
#define PACKETSIZE MAXTXSIZE + HEADERLEN	
#define PROMPTSTR "> Print the filename you want to download?(exit to quit)\n>"
typedef enum type_s {
	FILENAME,
	QUERYINFO,
	FILESIZE,
	CHUNKINFO,
	FILECHUNK,
	FILEERROR,
	SENDACK,
	CLIENTCLOSE,
}type_t;

typedef enum error_s {
	MEMERR,
}error_t;

typedef struct chuckinfo_s {
	unsigned long size;
	unsigned long off;
}chunkinfo_t;

typedef struct filechunk_s {
	unsigned long ident;
	char *chunkdata;
}filechunk_t;


typedef struct tlv_s {
	type_t buf_type;
	size_t buf_len;
	int    buf_err;
	union {
		off_t filesize; // For FILESIZE
		chunkinfo_t chunkinfo; // For CHUNKINFO
		filechunk_t filechunk; // For FILECHUNK
		char *errorstr; // For FILERROR
		char *filename; // For FILENAME
		char *ackstr; // For FILENAME
	}bufval_t;
	#define buf_fsize  bufval_t.filesize
	#define buf_error  bufval_t.errorstr
	#define buf_ident  bufval_t.filechunk.ident
	#define buf_data   bufval_t.filechunk.chunkdata
	#define buf_ckinfo bufval_t.chunkinfo.size
	#define buf_cksize bufval_t.chunkinfo.size
	#define buf_offset bufval_t.chunkinfo.off
	#define buf_fname  bufval_t.filename
	#define buf_ackstr  bufval_t.ackstr
}tlv_t;

void myfree(void *ptr);
void retrieveport(const char* portstr, unsigned long *portno);

/* validate the number of arguments and print useage 
 *  * if invalid 
 *   */
void validate_arg(int argc, int max, char *useage);
	 
/* check if ip address is valid */
int isipaddr(char *addr); 

/* check if the port number is valid */
int isport(unsigned long port); 

void freeall();
void myexit(const char *errstr);
int retrieve_buffer(char *buffer, tlv_t **buffstpp);
void create_buffer(type_t type, void *val, char *buffer);
void retrieve_port(const char* portstr, unsigned long *portno);
void send_ack(int sockfd, char *buffer, struct sockaddr *destaddrp);
int get_ack(int sockfd, char *buffer);


#endif
