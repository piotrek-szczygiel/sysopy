#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "conveyor_belt.h"
#include "error.h"
#include "pack.h"
#include "shared.h"
#include "utils.h"

static int mem_id;
static conveyor_belt_t* cb;

static sem_id_t q_sem;

void cleanup() {
  fflush(stdout);
  remove_semaphore(get_queue_key(), q_sem);
  unmap_shared(cb, CB_SIZE);
  remove_shared(get_trucker_key(), mem_id);
  printf("cleaned up!\n");
}

void handle_sigint(int sig) {
  exit(0);
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    printf("usage: %s truck_capacity belt_size belt_capacity\n", argv[0]);
    return 1;
  }

  atexit(cleanup);
  signal(SIGINT, handle_sigint);

  int truck_capacity;
  int belt_size;
  int belt_capacity;

  if (sscanf(argv[1], "%d", &truck_capacity) != 1 ||
      sscanf(argv[2], "%d", &belt_size) != 1 ||
      sscanf(argv[3], "%d", &belt_capacity) != 1) {
    err("invalid arguments passed");
  }

  mem_id = create_shared(get_trucker_key(), CB_SIZE);
  cb = map_shared(mem_id, CB_SIZE);

  cb->max_weight = belt_capacity;

  q_sem = create_semaphore(get_queue_key());
  cb->q = new_queue(belt_size, belt_capacity, get_queue_key());

  int current_weight = 0;

  pack_t pack;
  struct timeval timestamp;

  printf("empty truck arrived\n");

  while (1) {
    fflush(stdout);
    usleep(500);

    if (dequeue(&cb->q, q_sem, &pack) == NULL) {
      printf("conveyor belt is empty\n");
      continue;
    }

    gettimeofday(&timestamp, NULL);

    if (current_weight + pack.weight > truck_capacity) {
      lock_semaphore(q_sem);

      printf("truck is full, unloading truck...\n");
      current_weight = 0;

      sleep(1);

      printf("empty truck arrived\n");

      unlock_semaphore(q_sem);
    }

    if (pack.weight > truck_capacity) {
      err("capacity of truck is too small!");
    }

    current_weight += pack.weight;

    int elapsed = ((timestamp.tv_sec - pack.timestamp.tv_sec) * 1000000) +
                  (timestamp.tv_usec - pack.timestamp.tv_usec);

    printf("PID %7d  TIME %10dus  WEIGHT %3d  OCCUPIED %3d  FREE %3d\n",
           pack.pid, elapsed, pack.weight, current_weight,
           truck_capacity - current_weight);
  }

  return 0;
}
