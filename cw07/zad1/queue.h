#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include "shared.h"

#define CAPACITY 64
#define QUEUE_TYPE int

typedef struct queue_t {
  int size;

  int head;
  int tail;

  sem_id_t semaphore;

  QUEUE_TYPE array[CAPACITY];
} queue_t;

queue_t new_queue(sem_id_t semaphore);
int is_full(queue_t* q);
int is_empty(queue_t* q);
int enqueue(queue_t* q, QUEUE_TYPE item);
QUEUE_TYPE* dequeue(queue_t* q, QUEUE_TYPE* item);

#endif
