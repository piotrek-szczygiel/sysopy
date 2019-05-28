#include <stdio.h>
#include <unistd.h>
#include "error.h"
#include "net.h"

int main(int argc, char* argv[]) {
  printf("starting client...\n");

  // int cl = net_connect_local("server_sock");
  int cl = net_connect_network("127.0.0.1", 1337);

  char buf[] = "siemanko!\n";
  int sd;
  if ((sd = net_send(cl, buf, sizeof(buf))) > 0) {
    printf("sent %d bytes\n", sd);
  } else {
    err("error while sending");
  }

  close(cl);
  return 0;
}
