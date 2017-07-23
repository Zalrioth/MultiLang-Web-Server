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
#include <limits.h>
#include "datastructs/queue.c"
#include "datastructs/list.c"
#include "core/io.c"
#include "core/worker.c"

void *process_events(void *arguments);
int transmit_data(void *arguments, List *hostList);
void start_thread(WorkerData *threadHandle);
void stop_thread(WorkerData *threadHandle);
void add_host(void *listPointer, char *host, char *folder);

extern int rt_init();
extern int rt_term();
extern int initSettings(void *hostList, short *port, short *workers);

int main()
{
    short port = 80;
    short workers = 1;

    List *hostList = construct_list();

    rt_init();
    if (initSettings(hostList, &port, &workers) == 1)
    {
        perror("Exiting: Could not load settings.");
        return 1;
    }

    hostList->defaultFolder = check(hostList, "localhost");

    printf("Starting server on port: %d workers: %d\n", port, workers);
    struct workerData thread_args[workers];

    pthread_t tid[workers];

    int loopNum;
    for (loopNum = 0; loopNum < workers; loopNum++)
    {
        thread_args[loopNum].taskQueue = construct_queue();
        thread_args[loopNum].hostList = hostList;
        thread_args[loopNum].runThread = 0;
        pthread_mutex_init(&(thread_args[loopNum].mutex), NULL);
        pthread_cond_init(&(thread_args[loopNum].condition), NULL);
        pthread_create(&tid[loopNum], NULL, (void *)&process_events, (void *)&thread_args[loopNum]);
    }

    struct sockaddr_in dest;
    struct sockaddr_in serv;

    socklen_t socksize = sizeof(struct sockaddr_in);

    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(port);

    int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket < 0)
    {
        perror("Problem opening socket.");
        return 1;
    }

    int option = 1;
    setsockopt(tcpSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (bind(tcpSocket, (struct sockaddr *)&serv, sizeof(struct sockaddr)) < 0)
    {
        close(tcpSocket);
        perror("Problem binding socket.");
        return 1;
    }

    listen(tcpSocket, 128);

    while (1)
    {
        int clientCon = accept(tcpSocket, (struct sockaddr *)&dest, &socksize);

        //Problem accepting client
        if (clientCon == -1)
            continue;

        struct arg_struct *args = malloc(sizeof(struct arg_struct));
        bzero(args->request, BUFFER_SIZE);
        args->connection = clientCon;

        NODE_QUEUE *taskNode;
        taskNode = (NODE_QUEUE *)malloc(sizeof(NODE_QUEUE));
        taskNode->args = args;

        int chooseNum = 0;
        int sizeNum = INT_MAX;

        int loopNum;
        for (loopNum = 0; loopNum < workers; loopNum++)
        {
            if (is_empty(thread_args[loopNum].taskQueue))
            {
                chooseNum = loopNum;
                break;
            }
            else if (thread_args[loopNum].taskQueue->size < sizeNum)
            {
                chooseNum = loopNum;
                sizeNum = thread_args[loopNum].taskQueue->size;
            }
        }

        enqueue(thread_args[chooseNum].taskQueue, taskNode);
        start_thread(&thread_args[chooseNum]);
    }

    close(tcpSocket);
    rt_term();
    return 0;
}

void add_host(void *listPointer, char *host, char *folder)
{
    struct List *hostList = listPointer;

    char *hostHold = (char *)malloc(strlen(host) * sizeof(char));
    strcpy(hostHold, host);

    char *folderHold = (char *)malloc(strlen(folder) * sizeof(char));
    strcpy(folderHold, folder);

    NODE_LIST *hostNode;
    hostNode = (NODE_LIST *)malloc(sizeof(NODE_LIST));
    hostNode->key = hostHold;
    hostNode->value = folderHold;

    insert(hostList, hostNode);
}

void *process_events(void *arguments)
{
    struct workerData *thread_args = arguments;

    while (1)
    {
        Queue *taskQueue = thread_args->taskQueue;

        if (!is_empty(taskQueue))
        {
            NODE_QUEUE *taskNode = dequeue(taskQueue);

            transmit_data(taskNode->args, thread_args->hostList);
            free(taskNode);
        }
        else
        {
            stop_thread(thread_args);
        }
    }
}