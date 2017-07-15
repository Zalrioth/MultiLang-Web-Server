#define TRUE 1
#define FALSE 0

typedef struct Node_t
{
    struct arg_struct *args;
    struct Node_t *prev;
} NODE;

typedef struct Queue
{
    NODE *head;
    NODE *tail;
    int size;
} Queue;

Queue *ConstructQueue();
void DestructQueue(Queue *queue);
int Enqueue(Queue *pQueue, NODE *item);
NODE *Dequeue(Queue *pQueue);
int isEmpty(Queue *pQueue);

Queue *ConstructQueue()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

void DestructQueue(Queue *queue)
{
    NODE *pN;
    while (!isEmpty(queue))
    {
        pN = Dequeue(queue);
        free(pN);
    }
    free(queue);
}

int Enqueue(Queue *pQueue, NODE *item)
{
    item->prev = NULL;
    if (pQueue->size == 0)
    {
        pQueue->head = item;
        pQueue->tail = item;
    }
    else
    {
        pQueue->tail->prev = item;
        pQueue->tail = item;
    }
    pQueue->size++;
    return TRUE;
}

int isEmpty(Queue *pQueue)
{
    if (pQueue->size == 0)
        return TRUE;
    else
        return FALSE;
}

NODE *Dequeue(Queue *pQueue)
{
    NODE *item;
    if (isEmpty(pQueue))
        return NULL;
    item = pQueue->head;
    pQueue->head = (pQueue->head)->prev;
    pQueue->size--;
    return item;
}