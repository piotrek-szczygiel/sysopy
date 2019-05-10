#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "conveyor_belt.h"
#include "error.h"
#include "shared.h"
#include "utils.h"

static int mem_id;
static void* mem_ptr;
static int mem_size;

static conveyor_belt_t* cb;
static int* cb_items;

void cleanup() {
  unmap_shared(mem_ptr, mem_size);
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

  mem_size = sizeof(conveyor_belt_t) + sizeof(int) * K;
  mem_id = create_shared(get_trucker_key(), mem_size);
  mem_ptr = map_shared(mem_id, mem_size);

  cb = mem_ptr;
  cb_items = mem_ptr + sizeof(conveyor_belt_t);

  return 0;
}
