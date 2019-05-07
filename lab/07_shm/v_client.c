#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include "error.h"

#define SIZE 32

static int shmid;
static char* ptr;

void cleanup() {
  if (shmdt(ptr) == -1) {
    perr("unable to detach shared memory");
  }

  printf("cleaned up\n");
}

void handle_sigint(int sig) {
  exit(0);
}

int main(int argc, char* argv[]) {
  signal(SIGINT, handle_sigint);
  atexit(cleanup);

  srand(time(NULL));

  shmid = shmget(ftok(getenv("HOME"), 1337), SIZE, 0);
  if (shmid == -1) {
    perr("unable to get shared memory");
  }

  ptr = shmat(shmid, NULL, 0);
  if (ptr == (char*)-1) {
    perr("unable to map shared memory");
  }

  for (int i = 0; i < SIZE; ++i) {
    ptr[i] = 'a' + rand() % 26;
    sleep(rand() % 3);
  }

  cleanup();
  return 0;
}
