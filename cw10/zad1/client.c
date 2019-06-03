#include <arpa/inet.h>
#include <ctype.h>
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
  if (argc != 4) {
    err("usage: %s name net_type ip", argv[0]);
  }

  char* name = argv[1];
  char* ip = argv[3];

  int unx = 0;
  if (strcmp(argv[2], "net") == 0) {
    unx = 0;
  } else if (strcmp(argv[2], "unix") == 0) {
    unx = 1;
  } else {
    err("invalid network type, use 'net' or 'unix'");
  }

  int s;

  if (unx == 0) {
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
      perr("unable to create network socket");
    }

    struct sockaddr_in addr = {};
    addr.sin_addr.s_addr = inet_addr(ip);
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
    strcpy(addr.sun_path, ip);
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
      perr("unable to connect to server");
    }
  }

  proto_send(s, name, strlen(name));

  while (1) {
    char* data = proto_recv(s);

    int words = 0;

    char* text = data;
    while (*text != '\0') {
      if (isalpha(*text)) {
        words += 1;
        while (isalpha(*text))
          ++text;
      } else {
        ++text;
      }
    }
    free(data);
    printf("words: %d\n", words);
    char buffer[16];
    sprintf(buffer, "%d", words);
    proto_send(s, buffer, strlen(buffer));
  }

  close(s);
  return 0;
}
