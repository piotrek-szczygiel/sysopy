#ifndef LIB_FIND_H
#define LIB_FIND_H

int fnd_init(unsigned int element_max);
void fnd_free();

int fnd_prepare(const char* directory, const char* filename, const char* tmp);
int fnd_get_available();

int fnd_search();

char* fnd_get(unsigned int element);
void fnd_del(unsigned int element);

#endif
