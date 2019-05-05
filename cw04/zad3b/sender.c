#define _XOPEN_SOURCE 500
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "error.h"

#define SIG_TYPE_KILL 0
#define SIG_TYPE_QUEUE 1
#define SIG_TYPE_RT 2

#define SIG_RT_COUNT SIGRTMIN + 1
#define SIG_RT_END SIGRTMIN + 2

static pid_t pid;
static int type;

static int counter = 0;
static int sg_count;
static union sigval sg_val;

void signal_count(int sg) {
  ++counter;

  if (counter < sg_count) {
    if (type == SIG_TYPE_KILL) {
      kill(pid, SIGUSR1);
    } else if (type == SIG_TYPE_QUEUE) {
      sigqueue(pid, SIGUSR1, sg_val);
    } else if (type == SIG_TYPE_RT) {
      kill(pid, SIG_RT_COUNT);
    }
  } else {
    if (type == SIG_TYPE_KILL) {
      kill(pid, SIGUSR2);
    } else if (type == SIG_TYPE_QUEUE) {
      sigqueue(pid, SIGUSR2, sg_val);
    } else if (type == SIG_TYPE_RT) {
      kill(pid, SIG_RT_END);
    }
  }
}

void signal_end(int sg) {
  printf("sender: received %d signals\n", counter);
  exit(0);
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    err("usage: %s pid count type", argv[0]);
  }

  char* end;
  pid = strtol(argv[1], &end, 10);
  if (end == argv[1] || errno != 0) {
    perr("provided pid is not a valid number");
  }

  sg_count = strtol(argv[2], &end, 10);
  if (end == argv[2] || errno != 0) {
    perr("provided count is not a valid number");
  }

  type = SIG_TYPE_KILL;
  if (strcmp(argv[3], "kill") == 0) {
    type = SIG_TYPE_KILL;
  } else if (strcmp(argv[3], "queue") == 0) {
    type = SIG_TYPE_QUEUE;
  } else if (strcmp(argv[3], "rt") == 0) {
    type = SIG_TYPE_RT;
  } else {
    err("available types: kill, queue, rt");
  }

  sigset_t mask;
  sigfillset(&mask);
  if (type == SIG_TYPE_KILL || type == SIG_TYPE_QUEUE) {
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
  } else {
    sigdelset(&mask, SIG_RT_COUNT);
    sigdelset(&mask, SIG_RT_END);
  }

  if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
    perr("unable to block signals");
  }

  struct sigaction sa_count;
  memset(&sa_count, 0, sizeof(struct sigaction));
  sa_count.sa_handler = signal_count;

  struct sigaction sa_end;
  memset(&sa_end, 0, sizeof(struct sigaction));
  sa_end.sa_handler = signal_end;

  if (type == SIG_TYPE_KILL || type == SIG_TYPE_QUEUE) {
    sigaddset(&sa_count.sa_mask, SIGUSR1);
    sigaddset(&sa_count.sa_mask, SIGUSR2);
    sigaddset(&sa_end.sa_mask, SIGUSR1);
    sigaddset(&sa_end.sa_mask, SIGUSR2);
    sigaction(SIGUSR1, &sa_count, NULL);
    sigaction(SIGUSR2, &sa_end, NULL);
  } else {
    sigaddset(&sa_count.sa_mask, SIG_RT_COUNT);
    sigaddset(&sa_count.sa_mask, SIG_RT_END);
    sigaddset(&sa_end.sa_mask, SIG_RT_COUNT);
    sigaddset(&sa_end.sa_mask, SIG_RT_END);
    sigaction(SIG_RT_COUNT, &sa_count, NULL);
    sigaction(SIG_RT_END, &sa_end, NULL);
  }

  if (type == SIG_TYPE_KILL) {
    kill(pid, SIGUSR1);
  } else if (type == SIG_TYPE_QUEUE) {
    sigqueue(pid, SIGUSR1, sg_val);
  } else if (type == SIG_TYPE_RT) {
    kill(pid, SIG_RT_COUNT);
  }

  while (1) {
    sleep(1);
  }

  return 0;
}
