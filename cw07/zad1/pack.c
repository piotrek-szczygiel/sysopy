#include "pack.h"
#include <sys/time.h>
#include <unistd.h>

pack_t new_pack(int weight) {
  pack_t pack;
  pack.weight = weight;
  pack.pid = getpid();
  gettimeofday(&pack.timestamp, NULL);

  return pack;
}
