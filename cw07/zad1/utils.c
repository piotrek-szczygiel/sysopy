#include <stdlib.h>
#include <sys/ipc.h>

int get_trucker_key() {
  return ftok(getenv("HOME"), 1337);
}

int get_queue_key() {
  return ftok(getenv("HOME"), 666);
}
