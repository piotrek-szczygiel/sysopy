#ifndef DYN_FIND_H
#define DYN_FIND_H

#include <dlfcn.h>

static void* handle = NULL;

int (*_fnd_init)(unsigned int);
void (*_fnd_free)();
int (*_fnd_prepare)(const char*, const char*, const char*);
int (*_fnd_search)();
char* (*_fnd_get)(unsigned int);
void (*_fnd_del)(unsigned int);

void fnd_dynamic_init()
{
    handle = dlopen("libfind.so", RTLD_NOW);
    if (handle == NULL) {
        fprintf(stderr, "fnd_dynamic_load: unable to open libfind.so\n");
        return;
    }

    fprintf(stderr, "fnd_dynamic_load: successfully loaded library\n");

    _fnd_init = dlsym(handle, "fnd_init");
    _fnd_free = dlsym(handle, "fnd_free");
    _fnd_prepare = dlsym(handle, "fnd_prepare");
    _fnd_search = dlsym(handle, "fnd_search");
    _fnd_get = dlsym(handle, "fnd_get");
    _fnd_del = dlsym(handle, "fnd_del");

    char* error;
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
}

void fnd_dynamic_free()
{
    dlclose(handle);
}

int fnd_init(unsigned int element_max)
{
    return (*_fnd_init)(element_max);
}

void fnd_free()
{
    (*_fnd_free)();
}

int fnd_prepare(const char* directory, const char* filename, const char* prepare)
{
    return (*_fnd_prepare)(directory, filename, prepare);
}

int fnd_search()
{
    return (*_fnd_search)();
}

char* fnd_get(unsigned int element)
{
    return (*_fnd_get)(element);
}

void fnd_del(unsigned int element)
{
    (*_fnd_del)(element);
}

#endif
