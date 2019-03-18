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

void sort_sys(const char* filename, unsigned int elements, unsigned int block_size)
{
    int fd = open(filename, O_RDWR);
    if(fd < 0) {
        err("unable to open file %s: %s", filename, strerror(errno));
    }

    char *block1 = malloc(block_size);
    char *block2 = malloc(block_size);

    for(unsigned int i = 0; i < elements; ++i)
    {
        lseek(fd, i * block_size, SEEK_SET);

        if(read(fd, block1, block_size) < 0) {
            err("error while reading from file %s: %s", filename, strerror(errno));
        }

        unsigned int min_element = i;
        unsigned char min_value = block1[0];

        for(unsigned int j = i + 1; j < elements; ++j)
        {
            if(read(fd, block2, block_size) < 0) {
                err("error while reading from file %s: %s", filename, strerror(errno));
            }

            if((unsigned char) block2[0] < min_value) {
                min_element = j;
                min_value = block2[0];
            }
        }

        if(min_element != i) {
            printf("%d - %d; %02x - %02x\n", i, min_element, (unsigned char)block1[0], min_value);
            if(lseek(fd, min_element * block_size, SEEK_SET) < 0) {
                err("unable to seek to %d in %s: %s", min_element * block_size, filename, strerror(errno));
            }

            if(read(fd, block2, block_size) < 0) {
                err("error while reading from file %s: %s", filename, strerror(errno));
            }

            if(lseek(fd, i * block_size, SEEK_SET) < 0) {
                err("unable to seek to %d in %s: %s", i * block_size, filename, strerror(errno));
            }

            if(write(fd, block2, block_size) < 0) {
                err("error while writing to file %s: %s", filename, strerror(errno));
            }

            if(lseek(fd, min_element * block_size, SEEK_SET) < 0) {
                err("unable to seek to %d in %s: %s", min_element * block_size, filename, strerror(errno));
            }

            if(write(fd, block1, block_size) < 0) {
                err("error while writing to file %s: %s", filename, strerror(errno));
            }
        }
    }

    close(fd);
}

void sort_lib(const char* filename, unsigned int elements, unsigned int block_size)
{
}

void copy_sys(const char* src, const char* dst, unsigned int elements, unsigned int block_size)
{
}

void copy_lib(const char* src, const char* dst, unsigned int elements, unsigned int block_size)
{
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

            if(strcmp(argv[i + 4], "sys") == 0) {
                sort_sys(argv[i + 1], (unsigned int)atoi(argv[i + 2]), (unsigned int)atoi(argv[i + 3]));
            }
            else if(strcmp(argv[i + 4], "lib") == 0) {
                sort_lib(argv[i + 1], (unsigned int)atoi(argv[i + 2]), (unsigned int)atoi(argv[i + 3]));
            }
            else {
                err("unknown generate argument: %s", argv[i + 4]);
            }

            i += 4;
        } else if(strcmp(argv[i], "copy") == 0) {
            if(i + 5 >= argc) {
                err("not enough arguments for copy command");
            }

            if(strcmp(argv[i + 5], "sys") == 0) {
                copy_sys(argv[i + 1], argv[i + 2], (unsigned int)atoi(argv[i + 3]), (unsigned int)atoi(argv[i + 4]));
            }
            else if(strcmp(argv[i + 5], "lib") == 0) {
                copy_lib(argv[i + 1], argv[i + 2], (unsigned int)atoi(argv[i + 3]), (unsigned int)atoi(argv[i + 4]));
            }
            else {
                err("unknown generate argument: %s", argv[i + 5]);
            }

            i += 5;
        } else {
            err("unknown argument: %s", argv[i]);
        }
    }

    return 0;
}
