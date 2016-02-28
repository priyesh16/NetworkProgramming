#ifndef SERVER_H
#define SERVER_H


#include "common.h"

void start_log(char *filename);
void mallocgp();
void freegp();

void send_file_status();
int read_packet(int connno);

int parse_packet(int conn);
int get_ip(int conn);
int create_socket(unsigned short port);
int get_listen_socket(int sockfd);

void send_file_data();
int get_hostname(int conn);
int validate_hostname(int conn);
int validate_type(int conn); 
void print_loginfo(int conn , int flag);
void start_log(char *filename); 
#endif
