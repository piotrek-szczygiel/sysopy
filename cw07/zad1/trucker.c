#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
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
  unmap_shared(cb, sizeof(conveyor_belt_t));
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

  mem_id = create_shared(get_trucker_key(), sizeof(conveyor_belt_t));
  cb = map_shared(mem_id, sizeof(conveyor_belt_t));

  q_sem = create_semaphore(get_trucker_key() + 1);
  cb->q = new_queue(q_sem);

  enqueue(&cb->q, 10);
  enqueue(&cb->q, 20);
  enqueue(&cb->q, 30);

  printf("%d\n", cb->q.size);

  int a, b, c;
  dequeue(&cb->q, &a);
  dequeue(&cb->q, &b);
  dequeue(&cb->q, &c);

  printf("%d %d %d; %d\n", a, b, c, cb->q.size);

  return 0;
}
