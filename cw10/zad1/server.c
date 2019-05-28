#include <stdio.h>
#include <unistd.h>
#include "error.h"
#include "net.h"

int main(int argc, char* argv[]) {
  printf("starting server...\n");

  // int sv = net_listen_local("server_sock");
  int sv = net_listen_network(1337);

  int cl = net_accept(sv);

  char buf[128];
  int rd;
  if ((rd = net_read(cl, buf, 128)) > 0) {
    printf("received: %d bytes\n%s\n", rd, buf);
  } else {
    err("error while reading");
  }

  close(cl);
  close(sv);
  return 0;
}
