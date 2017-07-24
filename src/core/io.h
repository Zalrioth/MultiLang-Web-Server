#ifndef _IO_H_INCLUDED_
#define _IO_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../data/list.h"

#define BUFFER_SIZE 4096

struct arg_struct
{
    int connection;
    char request[BUFFER_SIZE];
};

extern void handle_client(int client, char *command, char *getFile, char *host);

int transmit_data(void *arguments, List *hostList);
int send_message(int client, char *message);
int send_data(int client, char *message, long length);
void shutdown_client(int client);

#endif /* _IO_H_INCLUDED_ */