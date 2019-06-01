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
  FILE* f = fopen("pan-tadeusz.txt", "rb");
  if (f == NULL) {
    err("unable to open file");
  }

  fseek(f, 0, SEEK_END);
  size_t text_size = ftell(f);
  rewind(f);

  printf("text size: %ld\n", text_size);

  char* text = malloc(text_size);
  if (fread(text, 1, text_size, f) != text_size) {
    perr("unable to read text");
  }

  fclose(f);

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
  addr_in.sin_port = htons(1337);

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
  strcpy(addr_un.sun_path, "/tmp/server_cluster");
  unlink(addr_un.sun_path);

  if (bind(sv_unx, (struct sockaddr*)&addr_un, sizeof(addr_un)) == -1) {
    perr("unable to bind unix socket");
  }

  if (listen(sv_unx, 5) == -1) {
    perr("unable to listen on unix socket");
  }

  fd_set active_fd_set;
  fd_set read_fd_set;

  FD_ZERO(&active_fd_set);
  FD_SET(sv_net, &active_fd_set);
  FD_SET(sv_unx, &active_fd_set);

  struct sockaddr_in new_addr = {};
  socklen_t size;
  while (1) {
    read_fd_set = active_fd_set;

    if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) == -1) {
      perr("select error");
    }

    for (int i = 0; i < FD_SETSIZE; ++i) {
      if (FD_ISSET(i, &read_fd_set)) {
        if (i == sv_net || i == sv_unx) {
          int new = accept(i, (struct sockaddr*)&new_addr, &size);
          if (new == -1) {
            perr("unable to accept client");
          }

          FD_SET(new, &active_fd_set);

          printf("client connected %s:%d\n", inet_ntoa(new_addr.sin_addr),
                 ntohs(new_addr.sin_port));

          proto_send(new, text, text_size);
        } else {
          char* buffer = proto_recv(i);
          if (buffer == NULL) {
            close(i);
            FD_CLR(i, &active_fd_set);
          } else {
            printf("data from client %d: %s\n", i, buffer);
            free(buffer);
          }
        }
      }
    }
  }

  close(sv_net);
  close(sv_unx);
  return 0;
}
