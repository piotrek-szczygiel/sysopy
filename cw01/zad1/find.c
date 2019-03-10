#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "find.h"

struct fnd_context {
    unsigned int element_max;

    char* directory;
    char* filename;
    char* tmp;

    char** table;
};

static struct fnd_context ctx;

int fnd_init(unsigned int element_max)
{
    ctx.table = (char**)calloc(element_max, sizeof(char*));
    if (ctx.table == NULL) {
        fprintf(stderr, "fnd_init: unable to allocate table");
        return -1;
    }

    ctx.element_max = element_max;
    return 0;
}

void fnd_free()
{
    free(ctx.directory);
    free(ctx.filename);
    free(ctx.tmp);

    for (unsigned int i = 0; i < ctx.element_max; ++i) {
        free(ctx.table[i]);
        ctx.table[i] = NULL;
    }
    free(ctx.table);
}

int fnd_prepare(const char* directory, const char* filename, const char* tmp)
{
    if (directory == NULL || filename == NULL || tmp == NULL) {
        fprintf(stderr, "fnd_prepare: invalid arguments\n");
        return -1;
    }

    ctx.directory = realloc(ctx.directory, strlen(directory) + 1);
    ctx.filename = realloc(ctx.filename, strlen(filename) + 1);
    ctx.tmp = realloc(ctx.tmp, strlen(tmp) + 1);

    if (ctx.directory == NULL || ctx.filename == NULL || ctx.tmp == NULL) {
        fprintf(stderr, "fnd_prepare: realloc failed\n");
    }

    strcpy(ctx.directory, directory);
    strcpy(ctx.filename, filename);
    strcpy(ctx.tmp, tmp);
    return 0;
}

int fnd_get_available()
{
    for (unsigned int i = 0; i < ctx.element_max; ++i) {
        if (ctx.table[i] == NULL) {
            return i;
        }
    }
    return -1;
}

int fnd_search()
{
    int element = fnd_get_available();
    if (element == -1) {
        fprintf(stderr, "fnd_search: max element count reached\n");
        return -1;
    }

    char* command = calloc(sizeof(char), 64 + strlen(ctx.directory) + strlen(ctx.filename) + strlen(ctx.tmp));
    if (command == NULL) {
        fprintf(stderr, "fnd_search: unable to allocate memory for command\n");
        return -1;
    }

    sprintf(command, "find \"%s\" -name \"%s\" > \"%s\" 2> /dev/null",
        ctx.directory, ctx.filename, ctx.tmp);
    system(command);

    int tmp_fd = open(ctx.tmp, O_RDONLY);
    int tmp_sz = lseek(tmp_fd, 0, SEEK_END);

    char* buf = calloc(sizeof(char), tmp_sz);

    lseek(tmp_fd, 0, SEEK_SET);
    if (read(tmp_fd, buf, tmp_sz) == -1) {
        fprintf(stderr, "fnd_search: unable to read from temporary file\n");
        return -1;
    }
    close(tmp_fd);

    buf[tmp_sz - 1] = '\0';

    ctx.table[element] = buf;
    return element;
}

char* fnd_get(unsigned int element)
{
    if (ctx.table[element] == NULL) {
        fprintf(stderr, "fnd_get: element is not present\n");
    }
    return ctx.table[element];
}

void fnd_del(unsigned int element)
{
    if (element < ctx.element_max) {
        free(ctx.table[element]);
        ctx.table[element] = NULL;
    }
}
