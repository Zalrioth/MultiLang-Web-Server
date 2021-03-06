#include "queue.h"

Queue *construct_queue()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

void destruct_queue(Queue *queueHandle)
{
    NODE_QUEUE *queueNode;
    while (!is_empty(queueHandle))
    {
        queueNode = dequeue(queueHandle);
        free(queueNode);
    }
    free(queueHandle);
}

int enqueue(Queue *queueHandle, NODE_QUEUE *item)
{
    item->prev = NULL;
    if (queueHandle->size == 0)
    {
        queueHandle->head = item;
        queueHandle->tail = item;
    }
    else
    {
        queueHandle->tail->prev = item;
        queueHandle->tail = item;
    }
    queueHandle->size++;
    return 1;
}

int is_empty(Queue *queueHandle)
{
    if (queueHandle->size == 0)
        return 1;
    return 0;
}

NODE_QUEUE *dequeue(Queue *queueHandle)
{
    NODE_QUEUE *item;
    if (is_empty(queueHandle))
        return NULL;
    item = queueHandle->head;
    queueHandle->head = (queueHandle->head)->prev;
    queueHandle->size--;
    return item;
}