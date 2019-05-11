#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "error.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    err("usage: %s workers max_weight");
  }

  int workers;
  int max_weight;

  if (sscanf(argv[1], "%d", &workers) != 1 ||
      sscanf(argv[2], "%d", &max_weight) != 1) {
    err("invalid arguments passed");
  }

  srand(time(NULL));

  char weight[12];
  for (int i = 0; i < workers; ++i) {
    sprintf(weight, "%d", rand() % max_weight + 1);

    switch (fork()) {
      case -1:
        perr("fork");
        break;
      case 0:
        execl("./loader", "./loader", weight, NULL);
        perr("unable to execute loader");
        break;
    }
  }

  int status;
  for (int i = 0; i < workers; ++i) {
    wait(&status);
    printf("loader %d finished with exit code %d", i, WEXITSTATUS(status));
  }

  return 0;
}
