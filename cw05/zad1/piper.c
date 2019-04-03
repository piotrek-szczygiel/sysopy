#define _XOPEN_SOURCE 500
#include "error.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_PROGRAMS 32
#define MAX_ARG_COUNT 16

char* trim(char* str)
{
    while (isspace(*str)) {
        ++str;
    }

    int i = strlen(str) - 1;
    while (i > 0 && isspace(str[i])) {
        str[i] = '\0';
    }

    return str;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        err("usage: %s file", argv[0]);
    }

    char* filename = argv[1];
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        perr("unable to open file %s", filename);
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = malloc(fsize + 1);
    if (fread(buffer, 1, fsize, f) != fsize) {
        perr("unable to read from file %s", filename);
    }

    fclose(f);
    buffer[fsize - 1] = 0;

    int cmd_count = 1;
    for (int i = 0; i < fsize; ++i) {
        if (buffer[i] == '|') {
            ++cmd_count;
        }
    }

    if (cmd_count > MAX_PROGRAMS) {
        err("too many programs (max: %d)", MAX_PROGRAMS);
    }

    char* commands[MAX_PROGRAMS][MAX_ARG_COUNT];
    for (int i = 0; i < MAX_PROGRAMS; ++i) {
        for (int j = 0; j < MAX_ARG_COUNT; ++j) {
            commands[i][j] = NULL;
        }
    }

    int i = 0;
    char* cmd_end;
    char* cmd = strtok_r(buffer, "|", &cmd_end);
    while (cmd != NULL) {
        int j = 0;
        char* arg_end;
        char* arg = strtok_r(cmd, " \t", &arg_end);
        while (arg != NULL) {
            commands[i][j] = arg;
            arg = strtok_r(NULL, " \t", &arg_end);
            ++j;
        }
        cmd = strtok_r(NULL, "|", &cmd_end);
        ++i;
    }

    int pipe_count = cmd_count - 1;
    int pipes[MAX_PROGRAMS][2];
    for (int i = 0; i < pipe_count; ++i) {
        if (pipe(pipes[i]) < 0) {
            perr("unable to create pipe no. %d", i + 1);
        }
    }

    for (int i = 0; i < cmd_count; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            perr("unable to fork");
        } else if (pid == 0) {

            if (i != cmd_count - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            if (i != 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            for (int j = 0; j < pipe_count; ++j) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(commands[i][0], commands[i]);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < pipe_count; ++i) {
        close(pipes[i][1]);
    }

    for (int i = 0; i < cmd_count; ++i) {
        wait(0);
    }

    return 0;
}
