#ifndef SYSTEMV_POSIX_H
#define SYSTEMV_POSIX_H

#include <semaphore.h>
#include <stddef.h>

#ifdef POSIX
typedef sem_t* sem_id_t;
#else
typedef int sem_id_t;
#endif

int create_shared(int key, size_t size);
int open_shared(int key, size_t size);
void* map_shared(int id, size_t size);
void unmap_shared(void* ptr, size_t size);
void remove_shared(int key, int id);

sem_id_t create_semaphore(int key);
sem_id_t open_semaphore(int key);
void lock_semaphore(sem_id_t id);
void unlock_semaphore(sem_id_t id);
void close_semaphore(sem_id_t id);
void remove_semaphore(int key, sem_id_t id);

#endif
