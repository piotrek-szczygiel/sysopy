#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "error.h"
#include "pack.h"
#include "queue.h"
#include "shared.h"
#include "utils.h"

static sem_id_t sem;
static queue_t* q;

static int weight;
static int last_result;

static pid_t pid;

void cleanup() {
  close_semaphore(sem);
  unmap_shared(q, Q_SIZE);
  printf("%d cleaned up!\n", pid);
}

void handle_sigint(int sig) {
  exit(0);
}

void place_pack() {
  pack_t pack = new_pack(weight);
  int result = enqueue(q, sem, pack);
  if (result == 0) {
    printf("%7d  %5ld %5ld  loaded %dkg\n", pid, pack.timestamp.tv_sec,
           pack.timestamp.tv_usec, weight);
  } else if (result == -1 && last_result != -1) {
    printf("%7d  conveyor belt is full\n", pid);
  } else if (result == -2 && last_result != -2) {
    printf("%7d  maximum conveyor belt weight reached\n", pid);
  }

  last_result = result;
}

int main(int argc, char* argv[]) {
  pid = getpid();

  int cycles = 0;

  if (argc == 2) {
    if (sscanf(argv[1], "%d", &weight) != 1) {
      err("inavlid argument passed");
    }
  } else if (argc == 2) {
    if (sscanf(argv[1], "%d", &weight) != 1 ||
        sscanf(argv[2], "%d", &cycles) != 1) {
      err("invalid arguments passed");
    }
  } else {
    err("usage: %s weight [cycles]\n", argv[0]);
  }

  int mem_id = open_shared(get_trucker_key(), Q_SIZE);
  q = map_shared(mem_id, Q_SIZE);
  sem = open_semaphore(q->sem_key);

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
