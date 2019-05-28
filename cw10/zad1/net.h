#ifndef NET_H
#define NET_H

#include <sys/socket.h>

int net_create(sa_family_t family);
int net_listen_local(const char* path);
int net_listen_network(int port);
int net_connect_local(const char* path);
int net_connect_network(const char* ip, int port);
int net_accept(int fd);
int net_read(int fd, char* buf, size_t sz);
int net_send(int fd, const char* buf, size_t sz);

#endif
