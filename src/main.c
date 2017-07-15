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
#include "queue.c"
#include "io.c"

typedef struct workerData
{
    Queue *pQ;
    int runThread;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} WorkerData;

void *processEvents(void *arguments);
void transmitData(void *arguments);
void startThread(WorkerData *threadHandle);
void stopThread(WorkerData *threadHandle);

extern int rt_init();
extern int rt_term();
extern int initSettings();
extern int getPort();
extern int getWorkers();

int main()
{
    short port;
    short workers;

    rt_init();
    initSettings();
    port = getPort();
    workers = getWorkers();

    printf("Starting server on port: %d workers: %d\n", port, workers);
    struct workerData thread_args[workers];

    pthread_t tid[workers];

    int loopNum;
    for (loopNum = 0; loopNum < workers; loopNum++)
    {
        thread_args[loopNum].pQ = ConstructQueue();
        thread_args[loopNum].runThread = 0;
        pthread_mutex_init(&(thread_args[loopNum].mutex), NULL);
        pthread_cond_init(&(thread_args[loopNum].condition), NULL);
        pthread_create(&tid[loopNum], NULL, (void *)&processEvents, (void *)&thread_args[loopNum]);
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
        perror("Problem opening socket");
        return 1;
    }

    int option = 1;
    setsockopt(tcpSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (bind(tcpSocket, (struct sockaddr *)&serv, sizeof(struct sockaddr)) < 0)
    {
        close(tcpSocket);
        perror("Problem binding socket");
        return 1;
    }

    listen(tcpSocket, 128);

    while (1)
    {
        int clientCon = accept(tcpSocket, (struct sockaddr *)&dest, &socksize);

        if (clientCon == -1)
        {
            perror("Problem accepting client");
            continue;
        }

        struct arg_struct *args = malloc(sizeof(struct arg_struct));
        bzero(args->request, BUFFER_SIZE);
        args->connection = clientCon;

        NODE *pN;
        pN = (NODE *)malloc(sizeof(NODE));
        pN->args = args;

        int chooseNum = 0;
        int sizeNum = INT_MAX;

        int loopNum;
        for (loopNum = 0; loopNum < workers; loopNum++)
        {
            if (isEmpty(thread_args[loopNum].pQ))
            {
                chooseNum = loopNum;
                break;
            }
            else if (thread_args[loopNum].pQ->size < sizeNum)
            {
                chooseNum = loopNum;
                sizeNum = thread_args[loopNum].pQ->size;
            }
        }

        Enqueue(thread_args[chooseNum].pQ, pN);
        startThread(&thread_args[chooseNum]);
    }

    close(tcpSocket);
    rt_term();
    return 0;
}

void *processEvents(void *arguments)
{
    struct workerData *thread_args = arguments;

    while (1)
    {
        Queue *pQ = thread_args->pQ;

        if (!isEmpty(pQ))
        {
            NODE *pN = Dequeue(pQ);
            transmitData(pN->args);
            free(pN);
        }
        else
        {
            stopThread(thread_args);
        }
    }
}

void startThread(WorkerData *threadHandle)
{
    pthread_mutex_lock(&(threadHandle->mutex));
    threadHandle->runThread = 1;
    pthread_cond_signal(&(threadHandle->condition));
    pthread_mutex_unlock(&(threadHandle->mutex));
}

void stopThread(WorkerData *threadHandle)
{
    pthread_mutex_lock(&(threadHandle->mutex));
    while (!threadHandle->runThread)
        pthread_cond_wait(&(threadHandle->condition), &(threadHandle->mutex));
    threadHandle->runThread = 0;
    pthread_mutex_unlock(&(threadHandle->mutex));
}