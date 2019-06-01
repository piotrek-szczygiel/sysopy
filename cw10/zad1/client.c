#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "error.h"
#include "proto.h"

int main(int argc, char* argv[]) {
  int unx = 0;

  int s;

  if (unx == 0) {
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
      perr("unable to create network socket");
    }

    struct sockaddr_in addr = {};
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1337);
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
      perr("unable to connect to server");
    }
  } else {
    s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == -1) {
      perr("unable to create unix socket");
    }

    struct sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/tmp/server_cluster");
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
      perr("unable to connect to server");
    }
  }

  char buffer[] = "message!";
  proto_send(s, buffer, sizeof(buffer));
  close(s);
  return 0;
}
