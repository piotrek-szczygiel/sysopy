#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "conveyor_belt.h"
#include "pack.h"
#include "shared.h"
#include "utils.h"

static sem_id_t sem;
static conveyor_belt_t* cb;

static int weight;

void cleanup() {
  close_semaphore(sem);
  unmap_shared(cb, CB_SIZE);
  printf("cleaned up!\n");
}

void handle_sigint(int sig) {
  exit(0);
}

void place_pack() {
  pack_t pack = new_pack(weight);
  int result = enqueue(&cb->q, sem, pack);
  if (result == 0) {
    printf("%5ld %5ld  loaded %d\n", pack.timestamp.tv_sec,
           pack.timestamp.tv_usec, weight);
  } else if (result == -1) {
    printf("conveyor belt is full\n");
  } else if (result == -2) {
    printf("maximum weight reached\n");
  }
}

int main(int argc, char* argv[]) {
  int cycles = 0;

  int show_usage = 0;
  if (argc == 2) {
    if (sscanf(argv[1], "%d", &weight) != 1) {
      show_usage = 1;
    }
  } else if (argc == 2) {
    if (sscanf(argv[1], "%d", &weight) != 1 ||
        sscanf(argv[2], "%d", &cycles) != 1) {
      show_usage = 1;
    }
  } else {
    show_usage = 1;
  }

  if (show_usage == 1) {
    printf("usage: %s weight [cycles]\n", argv[0]);
    return 1;
  }

  int mem_id = open_shared(get_trucker_key(), CB_SIZE);
  cb = map_shared(mem_id, CB_SIZE);
  sem = open_semaphore(cb->q.sem_key);

  atexit(cleanup);
  signal(SIGINT, handle_sigint);

  if (cycles == 0) {
    while (1) {
      place_pack();
      usleep(1000);
    }
  } else {
    for (int i = 0; i < cycles; ++i) {
      place_pack();
      usleep(1000);
    }
  }

  return 0;
}
