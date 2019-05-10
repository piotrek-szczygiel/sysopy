#include "queue.h"
#include "shared.h"

queue_t new_queue(sem_id_t semaphore) {
  queue_t q;
  q.size = 0;
  q.head = 0;
  q.tail = CAPACITY - 1;

  q.semaphore = semaphore;

  return q;
}

int is_full(queue_t* q) {
  lock_semaphore(q->semaphore);
  int result = q->size == CAPACITY;
  unlock_semaphore(q->semaphore);
  return result;
}

int is_empty(queue_t* q) {
  lock_semaphore(q->semaphore);
  int result = q->size == 0;
  unlock_semaphore(q->semaphore);
  return result;
}

int enqueue(queue_t* q, QUEUE_TYPE item) {
  int result;

  lock_semaphore(q->semaphore);

  if (q->size == CAPACITY) {
    result = -1;
  } else {
    q->tail = (q->tail + 1) % CAPACITY;
    q->array[q->tail] = item;
    q->size += 1;
    result = 0;
  }

  unlock_semaphore(q->semaphore);
  return result;
}

QUEUE_TYPE* dequeue(queue_t* q, QUEUE_TYPE* item) {
  lock_semaphore(q->semaphore);

  if (item == NULL || q->size == CAPACITY) {
    item = NULL;
  } else {
    *item = q->array[q->head];
    q->head = (q->head + 1) % CAPACITY;
    q->size -= 1;
  }

  unlock_semaphore(q->semaphore);
  return item;
}
