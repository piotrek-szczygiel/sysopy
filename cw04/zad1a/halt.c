#define _XOPEN_SOURCE 500
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int running = 1;

void handle_ctrl_c(int s)
{
    printf("\tReceived Ctrl+C - terminating\n");
    exit(0);
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

    time_t t;
    struct tm tm;

    while (1) {
        if (running) {
            time(&t);
            tm = *localtime(&t);
            printf("%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900,
                tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        }

        sleep(1);
    }

    return 0;
}
