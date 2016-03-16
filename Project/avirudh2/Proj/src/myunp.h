/*
*   File:       myunp.h
*
*   Purpose:    Header for wrapper functions of myunp.c
*/

#ifndef UNP_H
#define UNP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#define LISTENQ 1024
#define MAXLINE 4096

typedef struct sockaddr SA;

typedef struct thread_data_s{
    int thread_id;
    int offset_size;
    int chunk_size;
}thread_struct;

void err_quit(const char *format, ...);
void err_sys(const char *s, ...);
void bzero(void *p, size_t len);
int Socket(int family, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
int Close(int sockfd);
int Read(int sockfd, char *buffer, int bufferlen);
int Write(int sockfd, char *buffer, int bufferlen);

#endif