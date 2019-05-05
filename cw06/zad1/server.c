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

typedef struct client_t {
  int id;
  int queue;
} client_t;

static client_t clients[MAX_CLIENTS];

static int queue;

void cleanup() {
  msgctl(queue, IPC_RMID, NULL);
}

void handle_sigint(int sig) {
  exit(0);
}

int get_client_slot() {
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].id == -1)
      return i;
  }

  return -1;
}

int find_client(int id) {
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].id == id)
      return i;
  }

  return -1;
}

void send(int id, message_t* message) {
  int queue = find_client(id);
  if (queue == -1) {
    err("user does not exists: %d", id);
  }

  queue = clients[queue].queue;

  if ((msgsnd(queue, message, MESSAGE_SIZE, 0) == -1)) {
    perr("unable to send private message");
  }
}

int main(int argc, char* argv[]) {
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    clients[i].id = -1;
    clients[i].queue = -1;
  }

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

        int q;
        if ((q = msgget(key, 0)) == -1) {
          perr("unable to open client private queue");
        }

        int id = get_client_slot();
        if (id == -1) {
          message_t stop_message = new_message();
          stop_message.type = TYPE_STOP;
          msgsnd(q, &stop_message, MESSAGE_SIZE, 0);
          continue;
        }

        client_t client;
        client.id = id;
        client.queue = q;
        clients[id] = client;

        message.type = TYPE_INIT;
        sprintf(message.buffer, "%d", id);
        send(client.id, &message);

        printf("sent register confirmation for %d\n", id);
        break;
      }
      case TYPE_STOP: {
        int c = find_client(message.id);
        clients[c].id = -1;
        clients[c].queue = -1;

        printf("user %d disconnected\n", message.id);
        break;
      }
      case TYPE_ECHO: {
        send(message.id, &message);

        printf("echoed message back to %d: %s\n", message.id, message.buffer);
        break;
      }
      default: {
        printf("%ld#%d: %s\n", message.type, message.id, message.buffer);
        break;
      }
    }
  }

  return 0;
}
