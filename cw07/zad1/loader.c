#include <stdio.h>
#include <unistd.h>
#include "conveyor_belt.h"
#include "shared.h"
#include "utils.h"

int main(int argc, char* argv[]) {
  int mem_id = open_shared(get_trucker_key(), CB_SIZE);
  conveyor_belt_t* cb = map_shared(mem_id, CB_SIZE);
  sem_id_t sem = open_semaphore(cb->q.sem_key);

  for (int i = 0; i < 128; ++i) {
    enqueue(&cb->q, sem, i);
    sleep(1);
  }

  close_semaphore(sem);
  unmap_shared(cb, CB_SIZE);

  return 0;
}
