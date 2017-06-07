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

extern void handle_client(int client, char request[]);

extern int checkCache(int client, char request[]);

void *transmitData(void *arguments);

/*#define TRUE 1
#define FALSE 0*/

struct arg_struct {
    int arg1;
    char arg2[256];
};

int runMain(short port)
{
	printf("Hello World from C!\n");
    printf("Starting server on port: %d\n", port);

    char * defaultIndex = "/index.html";
    char * webRoot = "local";
    short workers = 4;

    pthread_t tid;

    struct sockaddr_in dest; /* socket info about the machine connecting to us */
    struct sockaddr_in serv; /* socket info about our server */
    int mysocket;            /* socket used to listen for incoming connections */
    socklen_t socksize = sizeof(struct sockaddr_in);

    memset(&serv, 0, sizeof(serv));           /* zero the struct before filling the fields */
    serv.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
    serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
    serv.sin_port = htons(port);              /* set the server port number */    

    mysocket = socket(AF_INET, SOCK_STREAM, 0);
  
    /* bind serv information to mysocket */
    bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

    /* start listening, allowing a queue of up to 1000 pending connection */
    listen(mysocket, 1000);
  
    while(1)
    {
        //struct arg_struct args;
        struct arg_struct *args = malloc(sizeof (struct arg_struct));

        //int consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);
        args->arg1 = accept(mysocket, (struct sockaddr *)&dest, &socksize);
        printf("Incoming connection from %s\n", inet_ntoa(dest.sin_addr));

        /*struct linger so_linger;
        so_linger.l_onoff = TRUE;
        so_linger.l_linger = 1;

        setsockopt(consocket, SOL_SOCKET, SO_LINGER, &so_linger, sizeof so_linger);*/

        char buffer[256];
        bzero(buffer,256);

        read(args->arg1,buffer,255);

        char command[256];
        bzero(command,256);

        char *pch;
        pch = strtok(buffer," ");
        strcpy(command, pch);

        pch = strtok(NULL, " ");

        //char getFile[256];
	    //bzero(getFile,256);
        //strcpy(getFile,pch);

        //printf("Client actualy wants to download: %s\n", getFile);

        

	    bzero(args->arg2,256);
        strcpy(args->arg2,pch);

        printf("Client %d actualy wants to download: %s\n", args->arg1, args->arg2);

        //args.arg1 = consocket;
        //args.arg2 = getFile;

        pthread_create(&tid, NULL, (void *)&transmitData, (void *)args);
    }
    printf("Done running server");
    return 0;
}

void *transmitData(void *arguments)
{
    struct arg_struct *args = arguments;
    printf("Client %d wants to download: %s\n", args->arg1, args->arg2);
    if (checkCache(args->arg1, args->arg2) == -1)
    {
        handle_client(args->arg1, args->arg2);
    }
    //shutdownClient(args->arg1);
    /*shutdown(args->arg1, SHUT_WR);
    char status = 1;
    char buffer[100];
    do {
        status = recv(args->arg1, buffer, 100, MSG_DONTWAIT);
    } while (status > 0);
    close(args->arg1);*/
}

void sendData(int client, char* message)
{
    write(client, message, strlen(message));
}

void sendMessage(int client, char* message, long length)
{
    //TODO: Split large files in smaller packets of size 8192
    write(client, message, length);

    /*const int packetSize = 1000;
    //int totalPackets = (length + packetSize - 1) / packetSize;
    int totalPackets = length/packetSize;
    int leftover = length - (totalPackets * packetSize);

    int num;

    char* looker = message;

    for (num = 0; num < totalPackets; num++)
    {
        //looker += 
        //*looker = '\0';
        write(client, message + (num * packetSize), length);
    }

    write(client, message + (totalPackets * packetSize), leftover);*/

    /*const int packetSize = 1000;

    //int totalPackets = ceil(length/packetSize);
    //int totalPackets = ceil((double)length / packetSize);
    int totalPackets = (length + packetSize - 1) / packetSize;

    printf("total packets %d", totalPackets);
    
    int num;

    for (num = 0; num < totalPackets; num++)
    {
        char buffer[packetSize];
        memset(buffer, '\0', packetSize);
        char* index = message + (packetSize * num);
        memcpy(buffer, index, packetSize);

        //buffer[packetSize-1] = '\0';

        write(client, buffer, length);
        printf("%s\n", buffer);
    }*/

    //write(client, message, length);
}

void shutdownClient(int client)
{
    //shutdown(client, SHUT_RDWR);

    shutdown (client, SHUT_WR);
    //sleep(0.5);
    char buffer[200];
    while(read(client, buffer, 200) > 0);
    close(client);
    pthread_exit(NULL);
    //shutdown (client, SHUT_RDWR);
    //close(client);
    //shutdown (client, SHUT_WR);
    /*char status = 1;
    char buffer[1];
    do {
        status = recv(client, buffer, 1, MSG_DONTWAIT);
    } while (status > 0);
    close(client);*/
    //shutdown (client, SHUT_RDWR);
    /*char buffer[4000];
    shutdown (client, SHUT_WR);
    for(;;) {
        char res=read(client, buffer, 4000);
        if(res < 0) {
            perror("reading");
            //exit(1);
        }
        if(!res)
            break;
    }*/
    /*char status = 1;
    char buffer[1];
    do {
        status = recv(client, buffer, 1, MSG_DONTWAIT);
    } while (status > 0);*/
    //close(client);
}

char* readFile(char* request, long* length)
{
    printf("read file is %s\n", request);
    FILE* fp = fopen(request, "r");
    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    rewind(fp);
    char* data = (char*) malloc(*length);
    fread(data, *length, 1, fp);
    fclose(fp);
    return data;
}