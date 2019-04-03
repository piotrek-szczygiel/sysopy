#include "error.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        err("usage: %s fifo");
    }

    char* fifo_name = argv[1];
    if (access(fifo_name, F_OK) != -1 && unlink(fifo_name) < 0) {
        perr("unable to remove existing %s", fifo_name);
    }

    if (mkfifo(fifo_name, 0644) < 0) {
        perr("unable to create %s", fifo_name);
    }

    int fifo = open(fifo_name, O_RDONLY);

    char data[1024];
    while (1) {
        int result = read(fifo, data, sizeof(data));
        if (result < 0) {
            perr("unable to read from %s", fifo_name);
        } else if (result == 0) {
            break;
        }
        data[result] = '\0';

        if (write(STDOUT_FILENO, data, result) < 0) {
            perr("unable to write to stdout");
        }
    }

    close(fifo);

    if (unlink(fifo_name) < 0) {
        perr("unable to remove %s", fifo_name);
    }

    return 0;
}
