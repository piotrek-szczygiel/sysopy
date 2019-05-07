#ifndef SYSTEMV_POSIX_H
#define SYSTEMV_POSIX_H

#include <stddef.h>

int create_shared(int key, size_t size);
void* map_shared(int id, size_t size);
int unmap_shared(void* ptr, size_t size);
int remove_shared(int key, int id);

#endif
