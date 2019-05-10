#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "conveyor_belt.h"
#include "error.h"
#include "shared.h"
#include "utils.h"

static int mem_id;
static conveyor_belt_t* cb;

static sem_id_t q_sem;

void cleanup() {
  fflush(stdout);
  remove_semaphore(get_trucker_key() + 1, q_sem);
  unmap_shared(cb, CB_SIZE);
  remove_shared(get_trucker_key(), mem_id);
  printf("cleaned up!\n");
}

void handle_sigint(int sig) {
  exit(0);
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    printf("usage: %s X K M\n", argv[0]);
    return 1;
  }

  atexit(cleanup);
  signal(SIGINT, handle_sigint);

  int X, K, M;
  X = K = M = 0;
  if (sscanf(argv[1], "%d", &X) != 1 || sscanf(argv[2], "%d", &K) != 1 ||
      sscanf(argv[3], "%d", &M) != 1) {
    err("invalid arguments passed");
  }

  mem_id = create_shared(get_trucker_key(), CB_SIZE);
  cb = map_shared(mem_id, CB_SIZE);

  cb->max_weight = M;

  q_sem = create_semaphore(get_trucker_key() + 1);
  cb->q = new_queue(K, get_trucker_key() + 1);

  int item;
  while (1) {
    for (int i = 0; i < MAX_CAPACITY; ++i) {
      if (peek(&cb->q, q_sem, i, &item) != NULL) {
        printf("%3d", item);
      } else {
        printf("%3d", -1);
      }
    }
    printf("\n");
    fflush(stdout);
    switch (getchar()) {
      case 'd':
        dequeue(&cb->q, q_sem, &item);
        break;
      case 'i':
        dequeue_index(&cb->q, q_sem, 2, &item);
        break;
    }
  }

  return 0;
}
