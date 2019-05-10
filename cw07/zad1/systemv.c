#include <stddef.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "error.h"
#include "shared.h"

int create_shared(int key, size_t size) {
  int id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0644);
  if (id == -1) {
    perr("unable to create shared memory");
  }

  return id;
}

int open_shared(int key, size_t size) {
  int id = shmget(key, size, 0);
  if (id == -1) {
    perr("unable to open shared memory");
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

void unmap_shared(void* ptr, size_t size) {
  if (shmdt(ptr) == -1) {
    perr("unable to unmap shared memory");
  }
}

void remove_shared(int key, int id) {
  if (shmctl(id, IPC_RMID, NULL) == -1) {
    perr("unable to remove shared memory");
  }
}

sem_id_t create_semaphore(int key) {
  sem_id_t id = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);
  if (id == -1) {
    perr("unable to create semaphore");
  }

  if (semctl(id, 0, SETVAL, 1) == -1) {
    perr("unable to initialize semaphore");
  }

  return id;
}

sem_id_t open_semaphore(int key) {
  sem_id_t id = semget(key, 1, 0);
  if (id == -1) {
    perr("unable to open semaphore");
  }

  return id;
}

void lock_semaphore(sem_id_t id) {
  struct sembuf sbuf;
  sbuf.sem_num = 0;
  sbuf.sem_op = -1;
  sbuf.sem_flg = 0;

  if (semop(id, &sbuf, 1) == -1) {
    perr("unable to lock semaphore");
  }
}

void unlock_semaphore(sem_id_t id) {
  struct sembuf sbuf;
  sbuf.sem_num = 0;
  sbuf.sem_op = 1;
  sbuf.sem_flg = 0;

  if (semop(id, &sbuf, 1) == -1) {
    perr("unable to lock semaphore");
  }
}

void close_semaphore(sem_id_t id) {
  return;
}

void remove_semaphore(int key, sem_id_t id) {
  if (semctl(id, 0, IPC_RMID) == -1) {
    perr("unable to remove semaphore");
  }
}
