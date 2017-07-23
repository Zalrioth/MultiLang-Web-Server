typedef struct workerData
{
    Queue *taskQueue;
    List *hostList;
    int runThread;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} WorkerData;

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