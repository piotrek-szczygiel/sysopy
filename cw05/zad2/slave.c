#define _XOPEN_SOURCE 500
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "error.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    err("usage: %s fifo count");
  }

  char* fifo_name = argv[1];

  char* end;
  long count = strtol(argv[2], &end, 10);
  if (end == argv[2] || errno != 0) {
    perr("%s is not a valid number", argv[2]);
  }

  pid_t pid = getpid();
  printf("%d\n", pid);

  struct stat sb;
  if (stat(fifo_name, &sb) == -1) {
    perr("unable to stat %s", fifo_name);
  }

  if (!S_ISFIFO(sb.st_mode)) {
    err("%s is not a fifo", fifo_name);
  }

  int fifo = open(fifo_name, O_WRONLY);
  if (fifo < 0) {
    perr("unable to open %s", fifo_name);
  }

  srand(time(NULL));
  char date[512];
  char buffer[512];
  for (int i = 0; i < count; ++i) {
    FILE* fdate = popen("date", "r");
    if (fgets(date, sizeof(date), fdate) == NULL) {
      close(fifo);
      perr("unable to read date");
    }
    pclose(fdate);

    int size = sprintf(buffer, "%d %s", pid, date);
    if (write(fifo, buffer, size) < 0) {
      perr("unable to write to %s", fifo_name);
    }

    sleep(2 + rand() % 4);
  }

  close(fifo);
  return 0;
}
