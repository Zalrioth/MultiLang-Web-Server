#ifndef _QUEUE_H_INCLUDED_
#define _QUEUE_H_INCLUDED_

#include <stdlib.h>

typedef struct Node_Queue
{
    struct arg_struct *args;
    struct Node_Queue *prev;
} NODE_QUEUE;

typedef struct Queue
{
    NODE_QUEUE *head;
    NODE_QUEUE *tail;
    int size;
} Queue;

Queue *construct_queue();
void Destruct_queue(Queue *queueHandle);
int enqueue(Queue *queueHandle, NODE_QUEUE *item);
NODE_QUEUE *dequeue(Queue *queueHandle);
int is_empty(Queue *queueHandle);

#endif /* _QUEUE_H_INCLUDED_ */