#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include "error.h"
#include "proto.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    err("usage: %s port path", argv[0]);
  }

  int port;
  char* unix_socket = argv[2];

  if (sscanf(argv[1], "%d", &port) != 1) {
    perr("unable to parse port: %s", argv[1]);
  }

  printf("starting server...\n");

  int sv_net = socket(AF_INET, SOCK_STREAM, 0);
  if (sv_net == -1) {
    perr("unable to create network socket");
  }

  int opt = 1;
  setsockopt(sv_net, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr_in = {};
  addr_in.sin_family = AF_INET;
  addr_in.sin_addr.s_addr = INADDR_ANY;
  addr_in.sin_port = htons(port);

  if (bind(sv_net, (struct sockaddr*)&addr_in, sizeof(addr_in)) == -1) {
    perr("unable to bind network socket");
  }

  if (listen(sv_net, 5) == -1) {
    perr("unable to listen on network socket");
  }

  int sv_unx = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sv_unx == -1) {
    perr("unable to create unix socket");
  }

  struct sockaddr_un addr_un = {};
  addr_un.sun_family = AF_UNIX;
  strcpy(addr_un.sun_path, unix_socket);
  unlink(addr_un.sun_path);

  if (bind(sv_unx, (struct sockaddr*)&addr_un, sizeof(addr_un)) == -1) {
    perr("unable to bind unix socket");
  }

  if (listen(sv_unx, 5) == -1) {
    perr("unable to listen on unix socket");
  }

  fd_set read_fd_set;
  fd_set write_fd_set;

  FD_ZERO(&read_fd_set);
  FD_ZERO(&write_fd_set);
  FD_SET(sv_net, &read_fd_set);
  FD_SET(sv_unx, &read_fd_set);
  FD_SET(0, &read_fd_set);

  char* text = NULL;
  char* text_ = NULL;
  bool calculate = false;
  int connected = 0;
  int sent = 0;

  while (1) {
    fd_set r = read_fd_set;
    fd_set w = write_fd_set;

    if (select(FD_SETSIZE, &r, &w, NULL, NULL) == -1) {
      perr("select error");
    }

    for (int i = 0; i < FD_SETSIZE; ++i) {
      if (FD_ISSET(i, &r)) {
        if (i == sv_net || i == sv_unx) {
          struct sockaddr_in new_addr = {};
          socklen_t size;

          int new = accept(i, (struct sockaddr*)&new_addr, &size);
          if (new == -1) {
            perr("unable to accept client");
          }

          FD_SET(new, &read_fd_set);
          FD_SET(new, &write_fd_set);
          connected += 1;

          printf("client connected %s:%d\n", inet_ntoa(new_addr.sin_addr),
                 ntohs(new_addr.sin_port));
        } else if (i == 0) {
          char path[128];
          size_t sz = read(0, path, sizeof(path));
          path[sz] = '\0';
          printf("calculating word count for file: %s\n", path);

          FILE* f = fopen("pan-tadeusz.txt", "rb");
          if (f == NULL) {
            err("unable to open file");
          }

          fseek(f, 0, SEEK_END);
          size_t text_size = ftell(f);
          rewind(f);

          printf("text size: %ld\n", text_size);

          text = malloc(text_size);
          if (fread(text, 1, text_size, f) != text_size) {
            printf("unable to open file: %s\n", path);
            free(text);
          } else {
            fclose(f);
            calculate = true;
          }
        } else {
          char* buffer = proto_recv(i);
          if (buffer == NULL) {
            close(i);
            FD_CLR(i, &read_fd_set);
            FD_CLR(i, &write_fd_set);
            connected -= 1;
          } else {
            printf("data from client %d: %s\n", i, buffer);
            free(buffer);
          }
        }
      }
    }

    if (calculate && sent < connected) {
      for (int i = 0; i < FD_SETSIZE; ++i) {
        if (FD_ISSET(i, &w)) {
          char* data = text_;
        }
      }
    }
  }

  close(sv_net);
  close(sv_unx);
  return 0;
}
