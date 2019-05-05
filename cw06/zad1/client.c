#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "error.h"
#include "message.h"
#include "tui.h"
#include "types.h"
#include "utils.h"

static int public_queue, private_queue;

void cleanup() {
  terminal_stop();
  msgctl(private_queue, IPC_RMID, NULL);
}

void handle_sigint(int sig) {
  exit(0);
}

int main(int argc, char* argv[]) {
  if ((public_queue = msgget(get_public_key(), 0)) == -1) {
    perr("unable to open queue");
  }

  atexit(cleanup);
  signal(SIGINT, handle_sigint);

  key_t private_key = get_private_key();
  if ((private_queue = msgget(private_key, IPC_CREAT | IPC_EXCL | 0600)) ==
      -1) {
    perr("unable to create queue");
  }

  terminal_start();
  draw_windows();

  message_t message;
  message.type = TYPE_INIT;
  sprintf(message.buffer, "%u", private_key);
  if ((msgsnd(public_queue, &message, MESSAGE_SIZE, 0) == -1)) {
    cleanup();
    err("unable to register");
  }
  INFO("registered with key %d", private_key);

  while (1) {
    tui_refresh();

    char buffer[MESSAGE_BUFFER_SIZE];
    int len = input(buffer, sizeof(buffer));

    message.type = 1;
    sprintf(message.buffer, "%s", buffer);

    if ((msgsnd(public_queue, &message, MESSAGE_SIZE, 0) == -1)) {
      cleanup();
      perr("unable to send message");
    }

    SENT("message %d: %s", len, buffer);
  }

  cleanup();
  return 0;
}
