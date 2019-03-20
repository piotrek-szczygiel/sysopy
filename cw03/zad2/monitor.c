#define _XOPEN_SOURCE 500
#include "error.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MONITOR_MAX 1024

static char* monitor_files[MONITOR_MAX];
static int monitor_intervals[MONITOR_MAX];
static int monitor_size = 0;

void parse_list(const char* list_filename)
{
    FILE* list_file = fopen(list_filename, "r");
    if(list_file == NULL) {
        perr("unable to open list file %s", list_filename);
    }

    while(1) {
        char* file_path = calloc(1, 4096);
        int interval;

        int result = fscanf(list_file, "%s%d", file_path, &interval);

        if(result == EOF) {
            free(file_path);
            break;
        } else if(result != 2) {
            err("error while scanning the list file");
        }

        monitor_files[monitor_size] = file_path;
        monitor_intervals[monitor_size] = interval;
        ++monitor_size;
    }
}

void monitor(int element)
{
    char* filename = monitor_files[element];
    int interval = monitor_intervals[element];

    printf("PID: %d\nmonitoring %s every %d seconds\n\n",
            getpid(), filename, interval);

    sleep(interval);
    exit(interval);
}

int main(int argc, char* argv[])
{
    if(argc != 4) {
        err("usage: %s [list] [timeout] [mode]", argv[0]);
    }

    char* list_filename = argv[1];
    char* backup_mode = argv[3];

    char* end;
    int timeout = strtol(argv[2], &end, 10);
    if(end == argv[2]) {
        err("cannot convert '%s' to a number", argv[2]);
    }

    parse_list(list_filename);

    pid_t children[MONITOR_MAX];

    for(int i = 0; i < monitor_size; ++i) {
        pid_t child_pid = fork();

        if(child_pid == -1) {
            perr("unable to fork");
        } else if(child_pid > 0) {
            children[i] = child_pid;
        } else {
            monitor(i);
            _exit(EXIT_FAILURE);
        }
    }

    sleep(timeout);
    for(int i = 0; i < monitor_size; ++i) {
        int status;
        waitpid(children[i], &status, 0);
        int copies = WEXITSTATUS(status);
        printf("children %d made %d copies\n", children[i], copies);
    }

    return 0;
}
