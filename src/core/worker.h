#ifndef _WORKER_H_INCLUDED_
#define _WORKER_H_INCLUDED_

#include <stdlib.h>
#include <pthread.h>

#include "../data/list.h"
#include "../data/queue.h"

#include "../core/io.h"

typedef struct workerData
{
    Queue *taskQueue;
    List *hostList;
    int runThread;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} WorkerData;

void *process_events(void *arguments);
void start_thread(WorkerData *threadHandle);
void stop_thread(WorkerData *threadHandle);

#endif /* _WORKER_H_INCLUDED_ */