#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


struct fnd_properties {
    size_t element_max;
    size_t element_next;

    char* directory;
    char* filename;
    char* tmp;

    char **table;
} fnd_ctx;


int fnd_init(size_t element_max) {
    fnd_ctx.table = (char**) calloc(element_max, sizeof(char*));
    if(fnd_ctx.table == NULL) {
        fprintf(stderr, "fnd_init: unable to allocate table");
        return -1;
    }
    fnd_ctx.element_max = element_max;
    fnd_ctx.element_next = 0;
    return 0;
}


void fnd_free() {
    free(fnd_ctx.directory);
    free(fnd_ctx.filename);
    free(fnd_ctx.tmp);

    for(size_t i = 0; i < fnd_ctx.element_max; ++i) {
        free(fnd_ctx.table[i]);
    }
    free(fnd_ctx.table);
}


int fnd_prepare(const char* directory, const char* filename, const char* tmp) {
    if(directory == NULL || filename == NULL || tmp == NULL) {
        fprintf(stderr, "fnd_prepare: invalid arguments\n");
        return -1;
    }

    fnd_ctx.directory = realloc(fnd_ctx.directory, strlen(directory) + 1);
    fnd_ctx.filename = realloc(fnd_ctx.filename, strlen(filename) + 1);
    fnd_ctx.tmp = realloc(fnd_ctx.tmp, strlen(tmp) + 1);

    if(fnd_ctx.directory == NULL || fnd_ctx.filename == NULL || fnd_ctx.tmp == NULL) {
        fprintf(stderr, "fnd_prepare: realloc failed\n");
    }

    strcpy(fnd_ctx.directory, directory);
    strcpy(fnd_ctx.filename, filename);
    strcpy(fnd_ctx.tmp, tmp);

    return 0;
}


int fnd_search() {
    if(fnd_ctx.element_next >= fnd_ctx.element_max) {
        fprintf(stderr, "fnd_search: max element count reached\n");
        return -1;
    }

    char *command = malloc(32 + strlen(fnd_ctx.directory) + strlen(fnd_ctx.filename) + strlen(fnd_ctx.tmp));
    if(command == NULL) {
        fprintf(stderr, "fnd_search: unable to allocate memory for command\n");
        return -1;
    }

    sprintf(command, "find \"%s\" -name \"%s\" > \"%s\"", fnd_ctx.directory, fnd_ctx.filename, fnd_ctx.tmp);
    puts(command);
    system(command);

    int tmp_fd = open(fnd_ctx.tmp, O_RDONLY);
    int tmp_sz = lseek(tmp_fd, 0, SEEK_END);

    char* buf = malloc(tmp_sz);

    lseek(tmp_fd, 0, SEEK_SET);
    if(read(tmp_fd, buf, tmp_sz) == -1) {
        fprintf(stderr, "fnd_search: unable to read from temporary file\n");
        return -1;
    }
    close(tmp_fd);

    buf[tmp_sz - 1] = '\0';

    fnd_ctx.table[fnd_ctx.element_next] = buf;
    return fnd_ctx.element_next++;
}


char* fnd_get(size_t element) {
    if(fnd_ctx.table[element] == NULL) {
        fprintf(stderr, "fnd_get: element is not present\n");
    }

    return fnd_ctx.table[element];
}


int main() {
    fnd_init(2);
    fnd_prepare("/home/piotr/", "find.txt", "/home/piotr/fnd_tmp.txt");
    int f1 = fnd_search();
    fnd_prepare("/home/piotr/", "find2.txt", "/home/piotr/fnd_tmp2.txt");
    int f2 = fnd_search();
    puts(fnd_get(f1));
    puts(fnd_get(f2));
    puts("xd");
    fnd_free();

    return 0;
}
