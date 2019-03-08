#ifndef LIB_FIND_H
#define LIB_FIND_H
#include <stddef.h>

int fnd_init(size_t element_max);
void fnd_free();

int fnd_prepare(const char* directory, const char* filename, const char* tmp);
int fnd_get_available();

int fnd_search();

char* fnd_get(size_t element);
void fnd_del(size_t element);

#endif

