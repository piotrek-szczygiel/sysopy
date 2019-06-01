#include "proto.h"
#include <arpa/inet.h>
#include <unistd.h>
#include "error.h"

void proto_send(int fd, const char* buffer, int32_t size) {
  int32_t nsize = htonl(size);
  const char* data = (char*)&nsize;

  int32_t left = sizeof(nsize);
  int rc;

  do {
    if ((rc = write(fd, data, left)) < 0) {
      perr("unable to send length");
    }

    data += rc;
    left -= rc;
  } while (left > 0);

  data = buffer;
  left = size;

  do {
    if ((rc = write(fd, data, left)) < 0) {
      perr("unable to send data");
    }

    data += rc;
    left -= rc;
  } while (left > 0);
}

int proto_recv(int fd, char* buffer) {
  int32_t size;
  char* data = (char*)&size;

  int32_t left = sizeof(size);
  int rc;

  do {
    if ((rc = read(fd, data, left)) < 0) {
      perr("unable to read length");
    }

    if (rc == 0) {
      return 0;
    }

    data += rc;
    left -= rc;
  } while (left > 0);

  size = ntohl(size);

  data = buffer;
  left = size;

  do {
    if ((rc = read(fd, data, left)) < 0) {
      perr("unable to read data");
    }

    if (rc == 0) {
      return 0;
    }

    data += rc;
    left -= rc;
  } while (left > 0);

  *data = '\0';
  return size;
}
