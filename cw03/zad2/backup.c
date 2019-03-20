#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "error.h"

char* get_backup_name(const char* filename)
{
    time_t rawtime;
    struct tm * timeinfo;
    char* buffer = malloc(4096);

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    char date[80];
    strftime(date,sizeof(date),"%Y-%m-%d_%H-%M-%S", timeinfo);
    sprintf(buffer, "archive/%s_%s", filename, date);
    return buffer;
}

void backup_exec(const char* filename)
{
    char* backup_name = get_backup_name(filename);

    pid_t child_pid = fork();

    if(child_pid == -1) {
        perr("unable to fork");
    } else if(child_pid > 0) {
        int status;
        waitpid(child_pid, &status, 0);
    } else {
        //execlp("cp", "cp", filename, ...);
        _exit(EXIT_FAILURE);
    }

    free(backup_name);
}

void backup_mem(const char* filename)
{
}
