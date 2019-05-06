#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "error.h"
#include "message.h"
#include "systemv_posix.h"
#include "tui.h"
#include "types.h"
#include "utils.h"

static int public_queue, private_queue;
static int id;

void cleanup() {
  terminal_stop();
  message_t message = new_message();
  message.type = TYPE_STOP;
  message.id = id;
  strcpy(message.buffer, "");
  send(public_queue, &message);
  remove_queue(private_queue, get_private_key());
}

void handle_sigint(int sig) {
  exit(0);
}

int register_client() {
  if ((public_queue = open_queue(get_public_key())) == -1) {
    ERROR("unable to connect to public queue!");
    return -1;
  }

  key_t private_key = get_private_key();

  if ((private_queue = create_queue(private_key)) == -1) {
    ERROR("unable to create private queue using private key: %d!", private_key);
    return -1;
  }

  INFO("created private queue: %d, using private key: %d", private_queue,
       private_key);

  message_t message = new_message();
  message.type = TYPE_INIT;
  sprintf(message.buffer, "%u", private_key);
  if (send(public_queue, &message) == -1) {
    ERROR("unable to send registration message!");
    return -1;
  }
  INFO("registering with key: %u", private_key);

  if (recv(private_queue, &message) == -1) {
    ERROR("unable to receive confirmation message!");
    return -1;
  }

  sscanf(message.buffer, "%u", &id);
  INFO("registered with id: %u", id);

  draw_id(id);

  if (set_nonblock(private_queue) == -1) {
    ERROR("unable to set private queue to non-blocking!");
    return -1;
  }

  return 0;
}

char* trim(char* str) {
  char* end;

  while (isspace((unsigned char)*str))
    str++;

  if (*str == 0)
    return str;

  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;

  end[1] = '\0';
  return str;
}

message_t parse_message(char* buffer) {
  message_t message = new_message();
  message.id = id;

  buffer = trim(buffer);

  char* type = buffer;

  char* sep = strchr(buffer, ' ');
  if (sep != NULL) {
    *sep = '\0';
    sprintf(message.buffer, "%s", sep + 1);
  }

  for (char* p = type; *p; ++p)
    *p = tolower(*p);

  if (strcmp(type, "read") == 0)
    message.type = TYPE_READ_FROM_FILE;
  else if (strcmp(type, "echo") == 0)
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
  else if (strcmp(type, "stop") == 0 || strcmp(type, "q") == 0)
    message.type = TYPE_STOP;
  else if (strcmp(type, "delay") == 0) {
    message.type = TYPE_UNKNOWN;
    sleep(1);
  } else {
    ERROR("unknown command: '%s'", type);
    message.type = TYPE_UNKNOWN;
  }

  return message;
}

void send_message(char* type, message_t message) {
  if (message.type == TYPE_UNKNOWN)
    return;

  if (message.type == TYPE_STOP)
    exit(0);

  if (send(public_queue, &message) == -1) {
    cleanup();
    perr("unable to send message");
  }

  switch (message.type) {
    case TYPE_2ALL: {
      add_message(COLOR_PAIR(PAIR_ALL), "ALL");
      break;
    }
    case TYPE_2ONE: {
      add_message(COLOR_PAIR(PAIR_ONE), "ONE");
      break;
    }
    case TYPE_2FRIENDS: {
      add_message(COLOR_PAIR(PAIR_FRIENDS), "FRIENDS");
      break;
    }
    case TYPE_ECHO: {
      add_message(COLOR_PAIR(PAIR_MESSAGE), "ECHO");
      break;
    }
  }

  switch (message.type) {
    case TYPE_2ALL:
    case TYPE_2ONE:
    case TYPE_2FRIENDS:
    case TYPE_ECHO: {
      SENT("> %s", message.buffer);
      break;
    }
    default:
      SENT("> %s %s", type, message.buffer);
      break;
  }
}

int main(int argc, char* argv[]) {
  atexit(cleanup);
  signal(SIGINT, handle_sigint);

  terminal_start();
  draw_windows();

  INFO("registering to server...");
  tui_refresh();

  while (register_client() == -1) {
    INFO("retrying in 2 seconds...");
    tui_refresh();
    if (getch() == 'q')
      exit(0);
    sleep(1);
    if (getch() == 'q')
      exit(0);
    sleep(1);
  }

  message_t message = new_message();
  char buffer[MESSAGE_BUFFER_SIZE];

  INFO("connected successfully\n");

  while (1) {
    tui_refresh();

    if (input(buffer, sizeof(buffer), id) > 0) {
      message = parse_message(buffer);
      if (message.type == TYPE_UNKNOWN) {
        continue;
      }

      if (message.type != TYPE_READ_FROM_FILE) {
        send_message(buffer, message);
      } else {
        FILE* fp;
        char* line = NULL;
        size_t len = 0;
        ssize_t read;

        fp = fopen(message.buffer, "r");
        if (fp == NULL) {
          cleanup();
          perr("unable to open file: %s", message.buffer);
        }

        while ((read = getline(&line, &len, fp)) != -1) {
          message = parse_message(line);
          send_message(line, message);
          tui_refresh();
        }

        fclose(fp);
      }
    }

    if (recv_nowait(private_queue, &message) == -1) {
      if (errno != ENOMSG && errno != EAGAIN) {
        perr("unable to receive message");
      }
    } else {
      struct tm timestamp = *localtime(&message.timestamp);
      add_message(COLOR_PAIR(PAIR_INFO) | A_DIM, "%02d:%02d:%02d ",
                  timestamp.tm_hour, timestamp.tm_min, timestamp.tm_sec);

      switch (message.type) {
        case TYPE_STOP: {
          exit(0);
          break;
        }
        case TYPE_ECHO: {
          add_message(COLOR_PAIR(PAIR_SUCCESS), "ECHO: ");
          RECV("%s", message.buffer);
          break;
        }
        case TYPE_LIST: {
          add_message(COLOR_PAIR(PAIR_MESSAGE), "Active users: ");
          char* list = strtok(message.buffer, " ");
          if (list == NULL) {
            add_message(COLOR_PAIR(PAIR_ERROR), "no active users\n");
            break;
          }

          add_message(COLOR_PAIR(PAIR_SUCCESS), "%s", list);
          list = strtok(NULL, " ");
          while (list != NULL) {
            add_message(COLOR_PAIR(PAIR_SUCCESS), ", %s", list);
            list = strtok(NULL, " ");
          }
          add_message(COLOR_PAIR(PAIR_DEFAULT), "\n");
          break;
        }
        case TYPE_FRIENDS: {
          add_message(COLOR_PAIR(PAIR_MESSAGE), "Your friends: ");
          char* friend = strtok(message.buffer, " ");
          if (friend == NULL) {
            add_message(COLOR_PAIR(PAIR_ERROR), "no friends\n");
            break;
          }

          add_message(COLOR_PAIR(PAIR_SUCCESS), "%s", friend);
          friend = strtok(NULL, " ");
          while (friend != NULL) {
            add_message(COLOR_PAIR(PAIR_SUCCESS), ", %s", friend);
            friend = strtok(NULL, " ");
          }
          add_message(COLOR_PAIR(PAIR_DEFAULT), "\n");
          break;
        }
        case TYPE_2ALL: {
          add_message(COLOR_PAIR(PAIR_ALL) | A_BOLD, "ALL ");
          add_message(COLOR_PAIR(PAIR_SUCCESS) | A_BOLD, "%d", message.id);
          RECV(": %s", message.buffer);
          break;
        }
        case TYPE_2FRIENDS: {
          add_message(COLOR_PAIR(PAIR_FRIENDS) | A_BOLD, "FRIENDS ");
          add_message(COLOR_PAIR(PAIR_SUCCESS) | A_BOLD, "%d", message.id);
          RECV(": %s", message.buffer);
          break;
        }
        case TYPE_2ONE: {
          add_message(COLOR_PAIR(PAIR_ONE) | A_BOLD, "ONE ");
          add_message(COLOR_PAIR(PAIR_SUCCESS) | A_BOLD, "%d", message.id);
          RECV(": %s", message.buffer);
          break;
        }
        default: {
          RECV("%s", message.buffer);
          break;
        }
      }
    }
  }

  cleanup();
  return 0;
}
