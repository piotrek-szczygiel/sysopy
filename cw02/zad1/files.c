#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void err(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(1);
}

void perr(const char *msg)
{
    perror(msg);
    exit(1);
}

void generate(const char* filename, unsigned int elements, unsigned int block_size)
{
    FILE *file = fopen(filename, "w");
    if(file == NULL) {
        err("unable to open file %s: %s\n", filename, strerror(errno));
    }

    int random_fd = open("/dev/urandom", O_RDONLY);
    if(random_fd < 0) {
        perr("unable to open /dev/urandom");
    }

    char* random_block = malloc(block_size);
    for(unsigned int i = 0; i < elements; ++i) {
        if(read(random_fd, random_block, block_size) < 0) {
            perr("error while reading /dev/urandom");
        }
        if(fwrite(random_block, 1, block_size, file) < 0) {
            err("error while writing to file %s: %s", filename, strerror(errno));
        }
    }
    close(random_fd);
    fclose(file);
}

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "generate") == 0) {
            if(i + 3 >= argc) {
                err("not enough arguments for generate command");
            }
            generate(argv[i + 1], (unsigned int)atoi(argv[i + 2]), (unsigned int)atoi(argv[i + 3]));
            i += 3;
        }  else if(strcmp(argv[i], "sort") == 0) {
            if(i + 4 >= argc) {
                err("not enough arguments for sort command");
            }

            i += 4;
        } else if(strcmp(argv[i], "copy") == 0) {
            if(i + 5 >= argc) {
                err("not enough arguments for copy command");
            }

            i += 5;
        } else {
            err("unknown argument: %s", argv[i]);
        }
    }

    return 0;
}
