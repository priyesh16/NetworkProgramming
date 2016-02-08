#include <common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define USAGE "\n ./myserver <port no (1-65535)> \n"
#define BACKLOG 1
#define MAXTXSIZE 10

char filename[MAXFILENAMESIZE];
long firstchunksize;
long otherchunksize;


void send_file_status(int sockfd, char *buffer);
type_t get_type(int sockfd, char *buffer);
void get_filename(int sockfd, char *buffer);
void get_chunk_info(int sockfd, char *buffer, chunkinfo_t *infop);
int get_listen_socket(unsigned long port);
void serv_free_all();
