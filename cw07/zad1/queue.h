#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include "shared.h"

#define MAX_CAPACITY 16
#define QUEUE_TYPE int

typedef struct queue_t {
  int size;
  int capacity;

  int head;
  int tail;

  int sem_key;

  QUEUE_TYPE array[MAX_CAPACITY];
} queue_t;

queue_t new_queue(int capacity, int sem_key);
int is_full(queue_t* q, sem_id_t sem);
int is_empty(queue_t* q, sem_id_t sem);
int enqueue(queue_t* q, sem_id_t sem, QUEUE_TYPE item);
QUEUE_TYPE* dequeue(queue_t* q, sem_id_t sem, QUEUE_TYPE* item);
QUEUE_TYPE* peek(queue_t* q, sem_id_t sem, int index, QUEUE_TYPE* item);
QUEUE_TYPE* dequeue_index(queue_t* q,
                          sem_id_t sem,
                          int index,
                          QUEUE_TYPE* item);

#endif
