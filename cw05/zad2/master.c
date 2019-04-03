#include "error.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        err("usage: %s fifo");
    }

    char* fifo_name = argv[1];
    if (access(fifo_name, F_OK) != -1 && remove(fifo_name) < 0) {
        perr("unable to remove existing %s", fifo_name);
    }

    if (mkfifo(fifo_name, 0644) < 0) {
        perr("unable to create %s", fifo_name);
    }

    FILE* fifo = fopen(fifo_name, "r");

    char line[1024];
    while (1) {
        if (feof(fifo) || fgets(line, sizeof(line), fifo) == NULL) {
            break;
        }

        printf("%s", line);
    }

    fclose(fifo);

    if (remove(fifo_name) < 0) {
        perr("unable to remove %s", fifo_name);
    }

    return 0;
}
