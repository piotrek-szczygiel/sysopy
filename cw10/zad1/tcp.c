#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "error.h"

int net_create(sa_family_t family) {
  int fd;
  if ((fd = socket(family, SOCK_STREAM, 0)) == -1) {
    perr("unable to create socket");
  }

  return fd;
}

int net_listen_local(const char* path) {
  int fd = net_create(AF_UNIX);

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));

  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
  unlink(path);

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perr("unable to bind local socket");
  }

  if (listen(fd, 5) == -1) {
    perr("unable to listen on local socket");
  }

  return fd;
}

int net_listen_network(int port) {
  int fd = net_create(AF_INET);

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perr("unable to bind network socket");
  }

  if (listen(fd, 5) == -1) {
    perr("unable to listen on network socket");
  }

  return fd;
}

int net_connect_local(const char* path) {
  int fd = net_create(AF_UNIX);

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perr("unable to connect to local server");
  }

  return fd;
}

int net_connect_network(const char* ip, int port) {
  int fd = net_create(AF_INET);

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perr("unable to connect to %s:%d", ip, port);
  }

  return fd;
}

int net_accept(int fd) {
  int cl;
  if ((cl = accept(fd, NULL, NULL)) == -1) {
    perr("unable to accept");
  }

  return cl;
}

int net_read(int fd, char* buf, size_t sz) {
  int rc = read(fd, buf, sz);
  if (rc == -1) {
    fprintf(stderr, "error while reading from socket\n");
  }

  return rc;
}

int net_send(int fd, const char* buf, size_t sz) {
  int sc = write(fd, buf, sz);
  if (sz > 0 && sc < 1) {
    fprintf(stderr, "error while writing to socket\n");
  }

  return sc;
}
