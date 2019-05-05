#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "error.h"
#include "message.h"
#include "types.h"
#include "utils.h"

#define MAX_CLIENTS 128
static size_t client_count = 0;
static int clients[MAX_CLIENTS];

static int queue;

void cleanup() {
  msgctl(queue, IPC_RMID, NULL);
}

void handle_sigint(int sig) {
  exit(0);
}

void send_private(size_t client, message_t* message) {
  if ((msgsnd(clients[client], message, MESSAGE_SIZE, 0) == -1)) {
    perr("unable to send private message");
  }
}

int main(int argc, char* argv[]) {
  atexit(cleanup);
  signal(SIGINT, handle_sigint);

  key_t key = get_public_key();
  if ((queue = msgget(key, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
    perr("unable to create queue");
  }

  message_t message;
  while (1) {
    if (msgrcv(queue, &message, MESSAGE_SIZE, -TYPE_LAST, 0) == -1) {
      perr("unable to receive message");
    }

    switch (message.type) {
      case TYPE_INIT: {
        key_t key;
        sscanf(message.buffer, "%u", &key);
        printf("user registered: %u\n", key);

        if ((clients[client_count] = msgget(key, 0)) == -1) {
          perr("unable to open client private queue");
        }

        message.type = TYPE_INIT;
        sprintf(message.buffer, "%lu", client_count);
        send_private(client_count, &message);

        printf("sent register confirmation for %lu\n", client_count);

        ++client_count;
        break;
      }
      default: {
        printf("%ld: %s\n", message.type, message.buffer);
        break;
      }
    }
  }

  return 0;
}
