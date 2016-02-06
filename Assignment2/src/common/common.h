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
#define MAXLINELEN 15 
#define OUTLEN 332
#define MAXBUFSIZE 100
#define MAXFILENAMESIZE 50
#define MEMERRSTR "Heap memory full..exiting"

typedef enum type_s {
	FILESIZE,
	FILECHUNK,
	FILEERROR,
}type_t;

typedef enum error_s {
	MEMERR,
}error_t;

typedef struct tlv_s {
	type_t buf_type;
	size_t buf_len;
	int    buf_err;
	union {
		off_t filesize; // For FILESIZE
		char *filechunk; // For FILECHUNK
		char *errorstr;
	}bufval_t;
	#define buf_fsize bufval_t.filesize
	#define buf_chunk bufval_t.filechunk
	#define buf_error bufval_t.errorstr
}tlv_t;



void retrieveport(const char* portstr, unsigned long *portno);

/* validate the number of arguments and print useage 
 *  * if invalid 
 *   */
void validatearg(int argc, int max, char *useage);
	 
/* check if ip address is valid */
int isipaddr(char *addr); 

/* check if the port number is valid */
int isport(unsigned long port); 



void freeall();
void myexit(const char *errstr);
int retrievebuffer(char *buffer, tlv_t **buffstpp);
void createbuffer(type_t type, void *val, char *buffer);


#endif
