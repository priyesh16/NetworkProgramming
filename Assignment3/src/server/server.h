#include <common.h>
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
void send_file_data();
void add_packet_ident(unsigned long i, char *data);
