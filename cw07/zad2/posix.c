#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include "error.h"
#include "systemv_posix.h"

int create_shared(int key, size_t size) {
  char path[32];
  sprintf(path, "/%d", key);

  int fd = shm_open(path, O_RDWR | O_CREAT | O_EXCL, 0644);
  if (fd == -1) {
    perr("unable to create shared memory");
    return -1;
  }

  int result = ftruncate(fd, size);
  if (result == -1) {
    perr("unable to resize shared memory to %dB", size);
  }

  return fd;
}

void* map_shared(int id, size_t size) {
  void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, id, 0);
  if (ptr == (void*)-1) {
    perr("unable to map shared memory");
  }

  return ptr;
}

int unmap_shared(void* ptr, size_t size) {
  int result = munmap(ptr, size);
  if (result == -1) {
    perr("unable to unmap shared memory");
  }

  return result;
}

int remove_shared(int key, int id) {
  char path[32];
  sprintf(path, "/%d", key);

  int result = shm_unlink(path);
  if (result == -1) {
    perr("unable to remove shared memory");
  }

  return result;
}
