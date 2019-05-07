#define _GNU_SOURCE
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "error.h"

#define SIZE 32
#define MEM_NAME "/memory"

static int fd;
static char* ptr;

void cleanup() {
  if (munmap(ptr, SIZE) == -1) {
    perr("unable to unmap shared memory");
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

  fd = shm_open(MEM_NAME, O_RDWR, 0644);
  if (fd == -1) {
    perr("unable to open shared memory");
  }

  ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    perr("unable to map shared memory");
  }

  while (1) {
    for (int i = 0; i < SIZE; ++i) {
      ptr[i] = 'a' + rand() % 26;
      sleep(rand() % 3);
    }
  }

  cleanup();
  return 0;
}
