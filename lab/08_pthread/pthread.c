#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"

#define NUM_WORKERS 10

static long block;

void* thread_worker(void* argv) {
  long pos = *(int*)argv;

  FILE* f = fopen("pan-tadeusz.txt", "r");
  fseek(f, pos, SEEK_SET);

  char* buf = malloc(block);
  fread(buf, block, 1, f);

  for (long i = 0; i < block; ++i) {
    if (isalpha(buf[i])) {
      buf[i] = toupper(buf[i]);
    }
  }

  buf[block - 1] = '\0';
  return buf;
}

int main(int argc, char* argv[]) {
  pthread_t workers[NUM_WORKERS] = {0};
  long positions[NUM_WORKERS] = {0};

  FILE* f = fopen("pan-tadeusz.txt", "r");
  if (f == NULL) {
    perr("unable to open pan-tadeusz.txt");
  }

  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  rewind(f);

  block = size / NUM_WORKERS;

  for (int i = 0; i < NUM_WORKERS; ++i) {
    positions[i] = block * i;
    if (pthread_create(&workers[i], NULL, thread_worker, &positions[i]) != 0) {
      perr("unable to create thread %d", i);
    }
  }

  for (int i = 0; i < NUM_WORKERS; ++i) {
    void* buf;
    if (pthread_join(workers[i], &buf) != 0) {
      perr("unable to join thread %d", i);
    }

    printf("WORKER %d:\n%s\n\n", i, (char*)buf);
  }

  return 0;
}
