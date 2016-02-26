#ifndef SERVER_H
#define SERVER_H


#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define USAGE "\n ./myserver <port no (1-65535)> \n"
#define BACKLOG 1

char filename[MAXFILENAMESIZE];
long firstchunksize;
long otherchunksize;
int globi;

void mallocgp();
void freegp();

void send_file_status();
int read_packet(int connno);
void get_filename();
void get_chunk_info();
void serv_free_all();

int parse_packet(int conn);
int get_ip(int conn);
int create_socket(unsigned long port);
int get_listen_socket();

int sockfd;
char *buffer;
chunkinfo_t *infogp;
struct sockaddr *destaddrgp;
socklen_t sockaddrsz;
void send_file_data();
void add_packet_ident(unsigned long i, char *data);
int get_hostname(int conn);
int validate_hostname(int conn);
int validate_type(int conn); 

#endif
