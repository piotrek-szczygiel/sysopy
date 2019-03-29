#define _XOPEN_SOURCE 500
#include "backup.h"
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
    if (list_file == NULL) {
        perr("unable to open list file %s", list_filename);
    }

    while (1) {
        char* file_path = malloc(4096);
        int interval;

        int result = fscanf(list_file, "%s%d", file_path, &interval);

        if (result == EOF) {
            free(file_path);
            break;
        } else if (result != 2) {
            err("error while scanning the list file");
        }

        monitor_files[monitor_size] = file_path;
        monitor_intervals[monitor_size] = interval;
        ++monitor_size;
    }

    fclose(list_file);
}

void monitor(int element)
{
    pid_t pid = getpid();
    char* filename = monitor_files[element];
    int interval = monitor_intervals[element];

    time_t modification;
    struct stat sb;

    if (lstat(filename, &sb) < 0) {
        fprintf(stderr, "PID: %d unable to lstat file %s\n", pid, filename);
        exit(0);
    }

    backup_mem_read(filename);

    modification = sb.st_mtime;

    int copies = 0;
    while (1) {
        if (lstat(filename, &sb) < 0) {
            fprintf(stderr, "PID: %d unable to lstat file %s\n",
                pid, filename);

            exit(0);
        }

        if (sb.st_mtime != modification) {
            modification = sb.st_mtime;
            backup_mem(filename);
            ++copies;
        }

        sleep(interval);
    }

    exit(copies);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        err("usage: %s [list]", argv[0]);
    }

    char* list_filename = argv[1];

    struct stat sb;
    if (lstat("archive", &sb) < 0) {
        if (mkdir("archive", 0755) == -1) {
            perr("cannot create archive directory");
        }
    } else {
        if (!S_ISDIR(sb.st_mode)) {
            err("cannot create archive directory: file exists");
        }
    }

    parse_list(list_filename);

    pid_t children[MONITOR_MAX] = { 0 };
    for (int i = 0; i < monitor_size; ++i) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perr("unable to fork");
        } else if (child_pid > 0) {
            children[i] = child_pid;
            printf("PID: %d monitoring %s\n", child_pid, monitor_files[i]);
        } else {
            monitor(i);
            _exit(EXIT_FAILURE);
        }
    }

    char input[256];
    char delimiters[] = " \r\n\t";
    char* word;

    while (1) {
        fgets(input, sizeof(input), stdin);

        word = strtok(input, delimiters);
        if (word == NULL) {
            continue;
        }

        if (strcmp(word, "list") == 0) {
            for (int i = 0; i < monitor_size; ++i) {
                printf("PID: %d monitoring %s\n",
                    children[i], monitor_files[i]);
            }
        } else if (strcmp(word, "end") == 0) {
            break;
        } else {
            int start = strcmp(word, "start");
            if(!(start || strcmp(word, "stop"))) {
                printf("unknown command\n");
                continue;
            }

            word = strtok(NULL, delimiters);
            if (word == NULL) {
                printf("you have to provide either PID or 'all'\n");
            } else {
                if (strcmp(word, "all") == 0) {
                    if (start == 0) {
                        // start all
                    } else {
                        // stop all
                    }
                } else {
                    char* end;
                    int pid = strtol(word, &end, 10);
                    if (end == word) {
                        printf("provided PID is not a valid number\n");
                    } else {
                        if (start == 0) {
                            // start pid
                        } else {
                            // stop pid
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < monitor_size; ++i) {
        int status;
        waitpid(children[i], &status, 0);
        int copies = WEXITSTATUS(status);
        printf("PID: %d made %d copies\n", children[i], copies);
        free(monitor_files[i]);
    }

    return 0;
}
