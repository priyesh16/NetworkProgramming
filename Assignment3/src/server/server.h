<<<<<<< HEAD
#ifndef SERVER_H
#define SERVER_H


=======
>>>>>>> 11e74c0cdc029b8f29f6e56cfb8c95f1a1251990
#include <common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define USAGE "\n ./myserver <port no (1-65535)> \n"
#define BACKLOG 1

char filename[MAXFILENAMESIZE];
long firstchunksize;
long otherchunksize;
<<<<<<< HEAD
int globi;
=======
>>>>>>> 11e74c0cdc029b8f29f6e56cfb8c95f1a1251990

void mallocgp();
void freegp();

void send_file_status();
type_t get_type();
void get_filename();
void get_chunk_info();
void serv_free_all();

void create_socket(unsigned long port);
int get_listen_socket();

int sockfd;
char *buffer;
chunkinfo_t *infogp;
struct sockaddr *destaddrgp;
socklen_t sockaddrsz;
<<<<<<< HEAD
void send_file_data();
void add_packet_ident(unsigned long i, char *data);


#endif
=======
>>>>>>> 11e74c0cdc029b8f29f6e56cfb8c95f1a1251990
