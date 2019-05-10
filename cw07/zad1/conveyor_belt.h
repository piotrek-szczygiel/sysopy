#ifndef CONVEYOR_BELT_H
#define CONVEYOR_BELT_H

#define CB_SIZE sizeof(conveyor_belt_t)

#include "queue.h"

typedef struct conveyor_belt_t {
  int max_weight;
  queue_t q;
} conveyor_belt_t;

#endif
