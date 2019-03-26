#define _XOPEN_SOURCE 500
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "error.h"

static int running = 1;
static pid_t child_pid;

void handle_ctrl_c(int s)
{
    printf("\tReceived Ctrl+C - terminating\n");
    exit(EXIT_SUCCESS);
}

void handle_ctrl_z(int s)
{
    if (running) {
        printf("\tCtrl+Z - continue\n\tCtrl+C - terminate\n");
    } else {
        printf("\n");
    }

    running = 1 - running;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, handle_ctrl_c);

    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handle_ctrl_z;
    sigaction(SIGTSTP, &sa, NULL);

    child_pid = fork();
    if(child_pid == -1) {
        perr("unable to fork");
    } else if(child_pid > 0) {
        printf("forked with PID: %d\n", child_pid);
    } else {
        execlp("bash", "bash", "date.sh", NULL);
        exit(EXIT_FAILURE);
    }

    int status;
    waitpid(child_pid, &status, 0);

    return 0;
}
