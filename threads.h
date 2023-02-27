#pragma once

#include "arrays.h"
#include <openssl/evp.h>

struct thread_data
{
    int thread_id;
    int length;
};

extern struct thread_data thread_data_array[];
extern pthread_mutex_t pass_mutex;
extern pthread_cond_t pass_signal;

void bytes2md5(const char *data, int len, char *md5buf);
void *onewordprod(void *threadarg);
void *twowordprod(void *threadarg);
void handle_sighup(int signo);
void *client(void *threadarg);