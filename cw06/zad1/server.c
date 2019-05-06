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

static int queues[MAX_CLIENTS];
static int friends[MAX_CLIENTS][MAX_CLIENTS];

static int queue;

void send(int id, message_t* message) {
  int queue = queues[id];
  if (queue == -1) {
    err("user does not exists: %d", id);
  }

  if ((msgsnd(queue, message, MESSAGE_SIZE, 0) == -1)) {
    perr("unable to send private message");
  }

  printf("sent message to %d: %ld$%s\n", id, message->type, message->buffer);
}

void cleanup() {
  message_t message = new_message();
  message.type = TYPE_STOP;
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (queues[i] != -1) {
      send(i, &message);
    }
  }

  msgctl(queue, IPC_RMID, NULL);
}

void handle_sigint(int sig) {
  exit(0);
}

int get_empty_id() {
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (queues[i] == -1)
      return i;
  }

  return -1;
}

void set_friends(int client_id, char* buffer, int exists) {
  char* friend = strtok(buffer, " ");
  while (friend != NULL) {
    int friend_id;
    sscanf(friend, "%d", &friend_id);
    friend = strtok(NULL, " ");

    if (client_id != friend_id) {
      friends[client_id][friend_id] = exists;
    }
  }

  message_t message = new_message();
  message.type = TYPE_FRIENDS;
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (friends[client_id][i] == 0)
      continue;
    sprintf(message.buffer + strlen(message.buffer), " %d", i);
  }

  send(client_id, &message);
}

int main(int argc, char* argv[]) {
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    queues[i] = -1;

    for (int j = 0; j < MAX_CLIENTS; ++j) {
      friends[i][j] = 0;
    }
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

        int id = get_empty_id();
        if (id == -1) {
          message_t stop_message = new_message();
          stop_message.type = TYPE_STOP;
          msgsnd(q, &stop_message, MESSAGE_SIZE, 0);
          continue;
        }

        queues[id] = q;

        message.type = TYPE_INIT;
        sprintf(message.buffer, "%d", id);
        send(id, &message);

        printf("sent register confirmation for %d\n", id);
        break;
      }
      case TYPE_STOP: {
        queues[message.id] = -1;
        printf("user %d disconnected\n", message.id);
        break;
      }
      case TYPE_ECHO: {
        send(message.id, &message);

        printf("echoed message back to %d: %s\n", message.id, message.buffer);
        break;
      }
      case TYPE_LIST: {
        strcpy(message.buffer, "");
        for (int i = 0; i < MAX_CLIENTS; ++i) {
          if (queues[i] != -1) {
            sprintf(message.buffer + strlen(message.buffer), " %d", i);
          }
        }

        send(message.id, &message);

        printf("sent user list to %d: %s\n", message.id, message.buffer);
        break;
      }
      case TYPE_FRIENDS: {
        for (int i = 0; i < MAX_CLIENTS; ++i) {
          friends[message.id][i] = 0;
        }
        set_friends(message.id, message.buffer, 1);
        break;
      }
      case TYPE_ADD: {
        set_friends(message.id, message.buffer, 1);
        break;
      }
      case TYPE_DEL: {
        set_friends(message.id, message.buffer, 0);
        break;
      }
      case TYPE_2ALL: {
        for (int i = 0; i < MAX_CLIENTS; ++i) {
          if (queues[i] != -1 && i != message.id) {
            send(i, &message);
          }
        }
        break;
      }
      case TYPE_2FRIENDS: {
        for (int i = 0; i < MAX_CLIENTS; ++i) {
          if (friends[message.id][i] == 1) {
            send(i, &message);
          }
        }
        break;
      }
      case TYPE_2ONE: {
        char* sep = strchr(message.buffer, ' ');
        if (sep != NULL) {
          *sep = '\0';
          int receiver;
          sscanf(message.buffer, "%d", &receiver);
          sprintf(message.buffer, "%s", sep + 1);
          send(receiver, &message);
        }
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
