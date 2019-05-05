#define _XOPEN_SOURCE 500
#include <ctype.h>
#include <errno.h>
#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "error.h"
#include "message.h"
#include "tui.h"
#include "types.h"
#include "utils.h"

static int public_queue, private_queue;
static int id;

void cleanup() {
  terminal_stop();
  message_t message;
  message.type = TYPE_STOP;
  message.id = id;
  strcpy(message.buffer, "");
  msgsnd(public_queue, &message, MESSAGE_SIZE, 0);
  msgctl(private_queue, IPC_RMID, NULL);
}

void handle_sigint(int sig) {
  exit(0);
}

int register_client() {
  if ((public_queue = msgget(get_public_key(), 0)) == -1) {
    ERROR("unable to connect to public queue!");
    return -1;
  }

  key_t private_key = get_private_key();
  if ((private_queue = msgget(private_key, IPC_CREAT | IPC_EXCL | 0600)) ==
      -1) {
    ERROR("unable to create private queue!");
    return -1;
  }

  message_t message = new_message();
  message.type = TYPE_INIT;
  sprintf(message.buffer, "%u", private_key);
  if ((msgsnd(public_queue, &message, MESSAGE_SIZE, 0) == -1)) {
    ERROR("unable to send registration message!");
    return -1;
  }
  INFO("registering with key: %u", private_key);

  if ((msgrcv(private_queue, &message, MESSAGE_SIZE, -TYPE_LAST, 0) == -1)) {
    ERROR("unable to receive confirmation message!");
    return -1;
  }

  sscanf(message.buffer, "%u", &id);
  INFO("registered with id: %u", id);
  return 0;
}

message_t parse_message(char* buffer) {
  message_t message = new_message();
  message.id = id;

  char* type = buffer;

  char* sep = strchr(buffer, ' ');
  if (sep != NULL) {
    *sep = '\0';
    sprintf(message.buffer, "%s", sep + 1);
  }

  for (char* p = type; *p; ++p)
    *p = tolower(*p);

  if (strcmp(type, "echo") == 0)
    message.type = TYPE_ECHO;
  else if (strcmp(type, "list") == 0)
    message.type = TYPE_LIST;
  else if (strcmp(type, "friends") == 0)
    message.type = TYPE_FRIENDS;
  else if (strcmp(type, "add") == 0)
    message.type = TYPE_ADD;
  else if (strcmp(type, "del") == 0)
    message.type = TYPE_DEL;
  else if (strcmp(type, "2all") == 0)
    message.type = TYPE_2ALL;
  else if (strcmp(type, "2friends") == 0)
    message.type = TYPE_2FRIENDS;
  else if (strcmp(type, "2one") == 0)
    message.type = TYPE_2ONE;
  else if (strcmp(type, "stop") == 0)
    message.type = TYPE_STOP;
  else {
    ERROR("unknown command: %s", type);
    message.type = TYPE_UNKNOWN;
  }

  return message;
}

int main(int argc, char* argv[]) {
  atexit(cleanup);
  signal(SIGINT, handle_sigint);

  terminal_start();
  draw_windows();

  INFO("registering to server...");
  tui_refresh();

  while (register_client() == -1) {
    INFO("retrying...");
    tui_refresh();
    sleep(3);
  }

  message_t message = new_message();
  char buffer[MESSAGE_BUFFER_SIZE];

  while (1) {
    tui_refresh();

    if (input(buffer, sizeof(buffer)) > 0) {
      message = parse_message(buffer);
      if (message.type == TYPE_UNKNOWN) {
        continue;
      }

      if (message.type == TYPE_STOP) {
        exit(0);
      }

      if ((msgsnd(public_queue, &message, MESSAGE_SIZE, 0) == -1)) {
        cleanup();
        perr("unable to send message");
      }

      SENT("> %s", message.buffer);
    }

    if ((msgrcv(private_queue, &message, MESSAGE_SIZE, -TYPE_LAST,
                IPC_NOWAIT) == -1)) {
      if (errno != ENOMSG) {
        perr("unable to receive message");
      }
    } else {
      switch (message.type) {
        case TYPE_ECHO:
          add_message(COLOR_PAIR(PAIR_SUCCESS), "ECHO: ");
          break;
      }
      RECV("%s", message.buffer);
    }
  }

  cleanup();
  return 0;
}
