#ifndef DYN_FIND_H
#define DYN_FIND_H
#include <dlfcn.h>


static void* handle = NULL;

int (*_fnd_init)(size_t);
void (*_fnd_free)();
int (*_fnd_prepare)(const char*, const char*, const char*);
int (*_fnd_search)();
char* (*_fnd_get)(size_t);
void (*_fnd_del)(size_t);


void fnd_dynamic_error() {
    char* error;
    if((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
}


void fnd_dynamic_init() {
    handle = dlopen("libfind.so", RTLD_LAZY);
    if(handle == NULL) {
        fprintf(stderr, "fnd_dynamic_load: unable to open libfind.so\n");
        return;
    }

    fprintf(stderr, "fnd_dynamic_load: successfully loaded library\n");

    _fnd_init    = dlsym(handle, "fnd_init");    fnd_dynamic_error();
    _fnd_free    = dlsym(handle, "fnd_free");    fnd_dynamic_error();
    _fnd_prepare = dlsym(handle, "fnd_prepare"); fnd_dynamic_error();
    _fnd_search  = dlsym(handle, "fnd_search");  fnd_dynamic_error();
    _fnd_get     = dlsym(handle, "fnd_get");     fnd_dynamic_error();
    _fnd_del     = dlsym(handle, "fnd_del");     fnd_dynamic_error();
}


void fnd_dynamic_free() {
    dlclose(handle);
}


int fnd_init(size_t element_max) {
    return (*_fnd_init)(element_max);
}


void fnd_free() {
    (*_fnd_free)();
}


int fnd_prepare(const char* directory, const char* filename, const char* prepare) {
    return (*_fnd_prepare)(directory, filename, prepare);
}


int fnd_search() {
    return (*_fnd_search)();
}


char* fnd_get(size_t element) {
    return (*_fnd_get)(element);
}


void fnd_del(size_t element) {
    (*_fnd_del)(element);
}
#endif
