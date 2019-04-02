#define _XOPEN_SOURCE 500
#include "error.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 4) {
        err("usage: %s command1 command2 file", argv[0]);
    }

    char* cmd1 = argv[1];
    char* cmd2 = argv[2];
    char* filename = argv[3];

    FILE* pipe_in = popen(cmd1, "r");
    if (pipe_in == NULL) {
        perr("unable to open pipe for '%s'", cmd1);
    }

    const size_t size = 1024 * 128;
    char buffer[size];

    size_t read_size = fread(buffer, 1, size, pipe_in);
    if (read_size <= 0) {
        perr("unable to read from process '%s'", cmd1);
    }
    fclose(pipe_in);

    int fd = open(filename, O_CREAT | O_WRONLY, 0644);
    if (fd < 0) {
        perr("unable to open '%s'", filename);
    }

    dup2(fd, 1); // stdout

    FILE* pipe_out = popen(cmd2, "w");
    if (pipe_out == NULL) {
        perr("unable to open pipe for '%s'", cmd2);
    }

    if (fwrite(buffer, 1, read_size, pipe_out) <= 0) {
        perr("unable to write to process '%s'", cmd2);
    }
    fclose(pipe_out);
    close(fd);
    return 0;
}
