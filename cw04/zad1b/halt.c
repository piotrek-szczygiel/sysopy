#define _XOPEN_SOURCE 500
#include "error.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static pid_t script_pid;
static int running = 1;

void fork_script()
{
    script_pid = fork();
    if (script_pid == -1) {
        perr("unable to fork");
    } else if (script_pid > 0) {
        printf("forked with PID: %d\n", script_pid);
    } else {
        execlp("bash", "bash", "date.sh", NULL);
        exit(EXIT_FAILURE);
    }
}

void handle_ctrl_c(int sg)
{
    printf("\tReceived Ctrl+C - terminating\n");
    exit(EXIT_SUCCESS);
}

void handle_ctrl_z(int sg)
{
    if (running == 1) {
        printf("\tCtrl+Z - continue\n\tCtrl+C - terminate\n");
        kill(script_pid, SIGKILL);
        running = 0;
    } else {
        fork_script();
        running = 1;
    }
}

int main(int argc, char* argv[])
{
    signal(SIGINT, handle_ctrl_c);

    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handle_ctrl_z;
    sigaction(SIGTSTP, &sa, NULL);

    fork_script();

    while (1) {
        sleep(1);
    }

    return 0;
}
