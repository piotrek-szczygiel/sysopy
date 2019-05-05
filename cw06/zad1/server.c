#include "error.h"
#include "message.h"
#include "utils.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

static int queue;

void cleanup()
{
    msgctl(queue, IPC_RMID, NULL);
}

void handle_sigint(int sig)
{
    exit(0);
}

int main(int argc, char* argv[])
{
    atexit(cleanup);
    signal(SIGINT, handle_sigint);

    key_t key = get_public_key();
    if ((queue = msgget(key, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        perr("unable to create queue");
    }

    message_t message;
    while (1) {
        if (msgrcv(queue, &message, MESSAGE_SIZE, -10, 0) == -1) {
            perr("unable to receive message");
        }

        printf("%ld: %s\n", message.type, message.buffer);
    }

    return 0;
}
