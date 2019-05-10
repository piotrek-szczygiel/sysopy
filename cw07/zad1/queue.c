#include "queue.h"

queue_t new_queue() {
  queue_t q;
  q.size = 0;
  q.head = 0;
  q.tail = CAPACITY - 1;

  return q;
}

int is_full(queue_t q) {
  return q.size == CAPACITY;
}

int is_empty(queue_t q) {
  return q.size == 0;
}

int enqueue(queue_t q, QUEUE_TYPE item) {
  if (is_full(q))
    return -1;

  q.tail = (q.tail + 1) % CAPACITY;
  q.array[q.tail] = item;
  q.size += 1;

  return 0;
}

int dequeue(queue_t q, QUEUE_TYPE* item) {
  if (item == NULL || is_empty(q))
    return -1;

  *item = q.array[q.head];
  q.head = (q.head + 1) % CAPACITY;
  q.size -= 1;

  return 0;
}
