#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include "error.h"

#define SIZE 32

static int shmid;
static char* ptr;

void cleanup() {
  if (shmdt(ptr) == -1) {
    perr("unable to unmap shared memory");
  }

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perr("unable to unlink shared memory");
  }

  printf("cleaned up\n");
}

void handle_sigint(int sig) {
  exit(0);
}

int main(int argc, char* argv[]) {
  signal(SIGINT, handle_sigint);
  atexit(cleanup);

  shmid = shmget(ftok(getenv("HOME"), 1337), SIZE, IPC_CREAT | 0666);
  if (shmid == -1) {
    perr("unable to create shared memory");
  }

  ptr = shmat(shmid, NULL, 0);
  if (ptr == (char*)-1) {
    perr("unable to map shared memory");
  }

  while (1) {
    for (int i = 0; i < SIZE; ++i) {
      putchar(ptr[i]);
    }

    putchar('\n');
    sleep(1);
  }

  cleanup();
  return 0;
}
