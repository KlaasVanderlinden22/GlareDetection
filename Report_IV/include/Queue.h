// *
// * Generic implementation of a queue.
// * Adapted from https://codereview.stackexchange.com/q/141238
// *

#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <string.h>

typedef struct Queue_
{
    size_t memSize;
    size_t size;
    struct QueueNode *head;
    struct QueueNode *tail;
} Queue;

int Queue_Dequeue(Queue *, void *);
int Queue_Enqueue(Queue *, const void *);
void Queue_Clear(Queue *);
int Queue_Empty(const Queue *);
int Queue_Init(Queue *, const size_t);
int Queue_Peek(const Queue *, void *);
size_t Queue_Size(const Queue *);

#endif
