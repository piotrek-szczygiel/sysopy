#define _GNU_SOURCE
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include "error.h"
#include "shared.h"

int create_shared(int key, size_t size) {
  char path[32];
  sprintf(path, "/%d", key);

  int id = shm_open(path, O_RDWR | O_CREAT | O_EXCL, 0644);
  if (id == -1) {
    perr("unable to create shared memory");
    return -1;
  }

  int result = ftruncate(id, size);
  if (result == -1) {
    perr("unable to resize shared memory to %dB", size);
  }

  return id;
}

void* map_shared(int id, size_t size) {
  void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, id, 0);
  if (ptr == (void*)-1) {
    perr("unable to map shared memory");
  }

  return ptr;
}

void unmap_shared(void* ptr, size_t size) {
  if (munmap(ptr, size) == -1) {
    perr("unable to unmap shared memory");
  }
}

void remove_shared(int key, int id) {
  char path[32];
  sprintf(path, "/%d", key);

  if (shm_unlink(path) == -1) {
    perr("unable to remove shared memory");
  }
}

sem_id_t create_semaphore(int key) {
  char path[32];
  sprintf(path, "/%d", key);

  sem_id_t id = sem_open(path, O_RDWR | O_CREAT | O_EXCL, 0644);
  if (id == (sem_id_t)-1) {
    perr("unable to create shared memory");
  }

  unlock_semaphore(id);
  return id;
}

void lock_semaphore(sem_id_t id) {
  if (sem_wait(id) == -1) {
    perr("unable to lock semaphore");
  }
}

void unlock_semaphore(sem_id_t id) {
  if (sem_post(id) == -1) {
    perr("unable to unlock semaphore");
  }
}

void close_semaphore(sem_id_t id) {
  if (sem_close(id) == -1) {
    perr("unable to close semaphore");
  }
}

void remove_semaphore(int key, sem_id_t id) {
  char path[32];
  sprintf(path, "/%d", key);

  if (sem_unlink(path) == -1) {
    perr("unable to remove semaphore");
  }
}
