#include "worker.h"

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

void start_thread(WorkerData *threadHandle)
{
    pthread_mutex_lock(&(threadHandle->mutex));
    threadHandle->runThread = 1;
    pthread_cond_signal(&(threadHandle->condition));
    pthread_mutex_unlock(&(threadHandle->mutex));
}

void stop_thread(WorkerData *threadHandle)
{
    pthread_mutex_lock(&(threadHandle->mutex));
    while (!threadHandle->runThread)
        pthread_cond_wait(&(threadHandle->condition), &(threadHandle->mutex));
    threadHandle->runThread = 0;
    pthread_mutex_unlock(&(threadHandle->mutex));
}