#include <stddef.h>
#include <sys/shm.h>
#include "error.h"
#include "systemv_posix.h"

int create_shared(int key, size_t size) {
  int id = shmget(key, size, IPC_CREAT | 0644);
  if (id == -1) {
    perr("unable to create shared memory");
  }

  return id;
}

void* map_shared(int id, size_t size) {
  void* ptr = shmat(id, NULL, 0);
  if (ptr == (void*)-1) {
    perr("unable to map shared memory");
  }

  return ptr;
}

int unmap_shared(void* ptr, size_t size) {
  int result = shmdt(ptr);
  if (result == -1) {
    perr("unable to unmap shared memory");
  }

  return result;
}

int remove_shared(int key, int id) {
  int result = shmctl(id, IPC_RMID, NULL);
  if (result == -1) {
    perr("unable to remove shared memory");
  }

  return result;
}
