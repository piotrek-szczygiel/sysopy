#define _XOPEN_SOURCE 500
#include "backup.h"
#include "error.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MONITOR_MAX 1024

static char* monitor_files[MONITOR_MAX];
static int monitor_intervals[MONITOR_MAX];
static int monitor_size = 0;

#define BACKUP_MEM 0
#define BACKUP_EXEC 1
static int backup_type;

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

    if (backup_type == BACKUP_EXEC) {
        backup_exec(filename);
    } else if (backup_type == BACKUP_MEM) {
        backup_mem_read(filename);
    }

    modification = sb.st_mtime;

    printf("PID: %d monitoring %s every %d seconds\n",
        pid, filename, interval);

    char* quit_event;

    int shmid;
    if ((shmid = shmget(1337, 1, IPC_CREAT | 0666)) < 0) {
        fprintf(stderr, "PID: %d unable to create shared memory\n", pid);
        exit(0);
    }

    if ((quit_event = shmat(shmid, NULL, 0)) == (char*)-1) {
        fprintf(stderr, "PID: %d unable to attach shared memory\n", pid);
        exit(0);
    }

    int copies = 0;
    while (*quit_event != 'y') {
        if (lstat(filename, &sb) < 0) {
            fprintf(stderr, "PID: %d unable to lstat file %s\n",
                pid, filename);

            exit(0);
        }

        if (sb.st_mtime != modification) {
            modification = sb.st_mtime;
            if (backup_type == BACKUP_EXEC) {
                backup_exec(filename);
            } else if (backup_type == BACKUP_MEM) {
                backup_mem(filename);
            }

            ++copies;
        }

        sleep(interval);
    }

    exit(copies);
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        err("usage: %s [list] [timeout] [mode]", argv[0]);
    }

    char* list_filename = argv[1];
    char* backup_mode = argv[3];
    if (strcmp(backup_mode, "mem") == 0) {
        backup_type = BACKUP_MEM;
    } else if (strcmp(backup_mode, "exec") == 0) {
        backup_type = BACKUP_EXEC;
    } else {
        err("valid backup modes: mem, exec");
    }

    char* end;
    int timeout = strtol(argv[2], &end, 10);
    if (end == argv[2]) {
        err("cannot convert '%s' to a number", argv[2]);
    }

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

    char* quit_event;

    int shmid;
    if ((shmid = shmget(1337, 1, IPC_CREAT | 0666)) < 0) {
        perr("unable to create shared memory");
    }

    if ((quit_event = shmat(shmid, NULL, 0)) == (char*)-1) {
        perr("unable to attach shared memory");
    }

    *quit_event = 'n';

    pid_t children[MONITOR_MAX];
    for (int i = 0; i < monitor_size; ++i) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perr("unable to fork");
        } else if (child_pid > 0) {
            children[i] = child_pid;
        } else {
            monitor(i);
            _exit(EXIT_FAILURE);
        }
    }

    sleep(timeout);
    *quit_event = 'y';
    for (int i = 0; i < monitor_size; ++i) {
        int status;
        waitpid(children[i], &status, 0);
        int copies = WEXITSTATUS(status);
        printf("PID: %d made %d copies\n", children[i], copies);
        free(monitor_files[i]);
    }

    return 0;
}
