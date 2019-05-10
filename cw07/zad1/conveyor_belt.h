#ifndef CONVEYOR_BELT_H
#define CONVEYOR_BELT_H

#include "queue.h"

typedef struct conveyor_belt_t {
  int max_weight;
  queue_t q;
} conveyor_belt_t;

#endif
