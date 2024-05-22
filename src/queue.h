#pragma once
#include "typedef.h"

// circular queue of addr_t elements
typedef struct queue {
    int head;
    int tail;
    int count; // number of elements in the queue
    int capacity; // maximum number of elements in the queue
    addr_t *data; // array of element
} queue_t;

queue_t *queue_create(int capacity);
void queue_destroy(queue_t *q);
int queue_is_empty(queue_t *q);
int queue_is_full(queue_t *q);
void queue_enqueue(queue_t *q, addr_t element);
addr_t queue_dequeue(queue_t *q);
