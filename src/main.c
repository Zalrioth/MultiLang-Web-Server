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

extern int rt_init();
extern int rt_term();
extern int runMain();
extern void getSettings(short *port, short *workers);
extern void collectGarbage();

void *processEvents(void *arguments);
void transmitData(void *arguments);

struct thread_struct
{
    Queue *pQ;
    int runThread;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
};

int main()
{
    printf("Hello World from C!\n");

    short port;
    short workers;

    long totalConnections = 0;

    rt_init();
    runMain();
    getSettings(&port, &workers);

    printf("Starting server on port: %d workers: %d\n", port, workers);

    struct thread_struct *thread_args[workers];

    pthread_t tid[workers];

    int loopNum;
    for (loopNum = 0; loopNum < workers; loopNum++)
    {
        thread_args[loopNum] = malloc(sizeof(struct thread_struct));
        thread_args[loopNum]->pQ = ConstructQueue();
        thread_args[loopNum]->runThread = 0;
        pthread_mutex_init(&(thread_args[loopNum]->mutex), NULL);
        pthread_cond_init(&(thread_args[loopNum]->condition), NULL);
        pthread_create(&tid[loopNum], NULL, (void *)&processEvents, (void *)thread_args[loopNum]);
    }

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

    int swap = 0;

    while (1)
    {
        struct arg_struct *args = malloc(sizeof(struct arg_struct));
        args->command = malloc(sizeof(char) * 256);
        args->request = malloc(sizeof(char) * 256);
        args->connection = accept(mysocket, (struct sockaddr *)&dest, &socksize);

        NODE *pN;
        pN = (NODE *)malloc(sizeof(NODE));
        pN->args = args;

        int chooseNum = 0;
        int sizeNum = INT_MAX;

        int loopNum;
        for (loopNum = 0; loopNum < workers; loopNum++)
        {
            if (isEmpty(thread_args[loopNum]->pQ))
            {
                EnqueueFirst(thread_args[loopNum]->pQ, pN);

                pthread_mutex_lock(&(thread_args[loopNum]->mutex));
                thread_args[loopNum]->runThread = 1;
                pthread_cond_signal(&(thread_args[loopNum]->condition));
                pthread_mutex_unlock(&(thread_args[loopNum]->mutex));

                goto next;
            }
            else if (thread_args[loopNum]->pQ->size < sizeNum)
            {
                chooseNum = loopNum;
                sizeNum = thread_args[loopNum]->pQ->size;
            }
        }

        Enqueue(thread_args[chooseNum]->pQ, pN);

        pthread_mutex_lock(&(thread_args[chooseNum]->mutex));
        thread_args[chooseNum]->runThread = 1;
        pthread_cond_signal(&(thread_args[chooseNum]->condition));
        pthread_mutex_unlock(&(thread_args[chooseNum]->mutex));

    next:;

        if (totalConnections % 100 == 0)
            collectGarbage();
        totalConnections++;
    }

    rt_term();
    return 0;
}

void *processEvents(void *arguments)
{
    struct thread_struct *thread_args = arguments;

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
            pthread_mutex_lock(&(thread_args->mutex));
            while (!thread_args->runThread)
                pthread_cond_wait(&(thread_args->condition), &(thread_args->mutex));
            thread_args->runThread = 0;
            pthread_mutex_unlock(&(thread_args->mutex));
        }
    }
}