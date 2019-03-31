#define _XOPEN_SOURCE 500
#include "error.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define SIG_TYPE_KILL 0
#define SIG_TYPE_QUEUE 1
#define SIG_TYPE_RT 2

#define SIG_RT_COUNT SIGRTMIN + 1
#define SIG_RT_END SIGRTMIN + 2

static int counter = 0;
static int type;

static union sigval sg_val;

void signal_handle(int sg, siginfo_t* info, void* ptr)
{
    if (sg == SIGUSR1 || sg == SIG_RT_COUNT) {
        ++counter;

        if (type == SIG_TYPE_KILL) {
            kill(info->si_pid, SIGUSR1);
        } else if (type == SIG_TYPE_QUEUE) {
            sigqueue(info->si_pid, SIGUSR1, sg_val);
        } else if (type == SIG_TYPE_RT) {
            kill(info->si_pid, SIG_RT_COUNT);
        }
    } else if (sg == SIGUSR2 || sg == SIG_RT_END) {
        if (type == SIG_TYPE_KILL) {
            kill(info->si_pid, SIGUSR2);
        } else if (type == SIG_TYPE_QUEUE) {
            sigqueue(info->si_pid, SIGUSR2, sg_val);
        } else if (type == SIG_TYPE_RT) {
            kill(info->si_pid, SIG_RT_END);
        }

        printf("catcher: received %d signals\n", counter);
        exit(0);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        err("usage: %s type", argv[0]);
    }

    if (strcmp(argv[1], "kill") == 0) {
        type = SIG_TYPE_KILL;
    } else if (strcmp(argv[1], "queue") == 0) {
        type = SIG_TYPE_QUEUE;
    } else if (strcmp(argv[1], "rt") == 0) {
        type = SIG_TYPE_RT;
    } else {
        err("available types: kill, queue, rt");
    }

    printf("starting catcher with PID %d\n", getpid());

    sigset_t mask;
    sigfillset(&mask);
    if (type == SIG_TYPE_KILL || type == SIG_TYPE_QUEUE) {
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);
    } else if (type == SIG_TYPE_RT) {
        sigdelset(&mask, SIG_RT_COUNT);
        sigdelset(&mask, SIG_RT_END);
    }

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        perr("unable to block signals");
    }

    struct sigaction sa_handle;
    memset(&sa_handle, 0, sizeof(struct sigaction));
    sa_handle.sa_flags = SA_SIGINFO;
    sa_handle.sa_sigaction = signal_handle;
    sigemptyset(&sa_handle.sa_mask);

    if (type == SIG_TYPE_KILL || type == SIG_TYPE_QUEUE) {
        sigaddset(&sa_handle.sa_mask, SIGUSR1);
        sigaddset(&sa_handle.sa_mask, SIGUSR2);
        sigaction(SIGUSR1, &sa_handle, NULL);
        sigaction(SIGUSR2, &sa_handle, NULL);
    } else if (type == SIG_TYPE_RT) {
        sigaddset(&sa_handle.sa_mask, SIG_RT_COUNT);
        sigaddset(&sa_handle.sa_mask, SIG_RT_END);
        sigaction(SIG_RT_COUNT, &sa_handle, NULL);
        sigaction(SIG_RT_END, &sa_handle, NULL);
    }

    while (1) {
        sleep(1);
    }

    return 0;
}
