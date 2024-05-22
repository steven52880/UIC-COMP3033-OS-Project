#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

queue_t *queue_create(int capacity)
{
    queue_t *queue = (queue_t *)malloc(sizeof(queue_t));
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    queue->capacity = capacity;
    queue->data = (addr_t *)malloc(sizeof(addr_t) * capacity);
    return queue;
}

void queue_destroy(queue_t *q)
{
    free(q->data);
    free(q);
}

int queue_is_empty(queue_t *q)
{
    return q->count == 0;
}

int queue_is_full(queue_t *q)
{
    return q->count == q->capacity;
}

void queue_enqueue(queue_t *q, addr_t element)
{
    if (queue_is_full(q))
    {
        fprintf(stderr, "Queue is full\n");
        return;
    }

    q->data[q->tail] = element;
    q->tail = (q->tail + 1) % q->capacity;
    q->count++;
}

addr_t queue_dequeue(queue_t *q)
{
    if (queue_is_empty(q))
    {
        fprintf(stderr, "Queue is empty\n");
        return -1;
    }

    addr_t element = q->data[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->count--;

    return element;
}