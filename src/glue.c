#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

//https://stackoverflow.com/questions/5559250/c-error-undefined-reference-to-function-but-it-is-defined
//https://stackoverflow.com/questions/1559487/how-to-empty-a-char-array
//https://stackoverflow.com/questions/14002954/c-programming-how-to-read-the-whole-file-contents-into-a-buffer
//http://zserge.com/jsmn.html
//http://en.cppreference.com/w/c/experimental/dynamic/strndup
//http://blog.abhijeetr.com/2010/04/very-simple-http-server-writen-in-c.html
//https://stackoverflow.com/questions/1352749/multiple-arguments-to-function-called-by-pthread-create
//https://stackoverflow.com/questions/35857488/fread-is-not-reading-other-file-formats
//https://stackoverflow.com/questions/8874021/close-socket-directly-after-send-unsafe
//https://stackoverflow.com/questions/4160347/close-vs-shutdown-socket
//http://www.gnu.org/software/libc/manual/html_node/Closing-a-Socket.html
//https://blog.netherlabs.nl/articles/2009/01/18/the-ultimate-so_linger-page-or-why-is-my-tcp-not-reliable
//https://stackoverflow.com/questions/4214314/get-a-substring-of-a-char
//http://www.ccplusplus.com/2011/09/solinger-example.html
//https://stackoverflow.com/questions/8874021/close-socket-directly-after-send-unsafe
//https://github.com/labcoder/simple-webserver/blob/master/server.c
//http://blog.manula.org/2011/05/writing-simple-web-server-in-c.html
s
extern void handle_client(int client, char request[]);
extern int checkCache(int client, char request[]);

void *transmitData(void *arguments);

struct arg_struct {
    int arg1;
    char arg2[256];
};

int runMain(short port)
{
	printf("Hello World from C!\n");
    printf("Starting server on port: %d\n", port);

    pthread_t tid;

    struct sockaddr_in dest;
    struct sockaddr_in serv;
    int mysocket;
    socklen_t socksize = sizeof(struct sockaddr_in);

    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(port);

    mysocket = socket(AF_INET, SOCK_STREAM, 0);
  
    bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));
    listen(mysocket, 1000);
  
    while(1)
    {
        struct arg_struct *args = malloc(sizeof (struct arg_struct));

        args->arg1 = accept(mysocket, (struct sockaddr *)&dest, &socksize);
        printf("Incoming connection from %s\n", inet_ntoa(dest.sin_addr));

        char buffer[256];
        bzero(buffer,256);

        read(args->arg1,buffer,255);

        char command[256];
        bzero(command,256);

        char *pch;
        pch = strtok(buffer," ");
        strcpy(command, pch);

        pch = strtok(NULL, " ");

	    bzero(args->arg2,256);
        strcpy(args->arg2,pch);

        pthread_create(&tid, NULL, (void *)&transmitData, (void *)args);
    }
    printf("Done running server");
    return 0;
}

void *transmitData(void *arguments)
{
    struct arg_struct *args = arguments;
    if (checkCache(args->arg1, args->arg2) == -1)
    {
        handle_client(args->arg1, args->arg2);
    }
}

void sendMessage(int client, char* message)
{
    write(client, message, strlen(message));
}

void sendData(int client, char* message, long length)
{
    write(client, message, length);
}

void shutdownClient(int client)
{
    shutdown (client, SHUT_WR);
    char buffer[200];
    while(read(client, buffer, 200) > 0);
    close(client);
    pthread_exit(NULL);
}

char* readFile(char* request, long* length)
{
    printf("Read file: %s\n", request);
    FILE* fp = fopen(request, "r");
    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    rewind(fp);
    char* data = (char*) malloc(*length);
    fread(data, *length, 1, fp);
    fclose(fp);
    return data;
}