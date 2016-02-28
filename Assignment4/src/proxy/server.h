#ifndef CLIUTIL_H
#define CLIUTIL_H

#include "common.h"

#define MAXADDRSIZE 50

int connect_client(int conn);
void Pthread_create(pthread_t *tid, const pthread_attr_t *attr,
               void * (*func)(void *), void *arg);
void Pthread_join(pthread_t tid, void **status);
#endif
