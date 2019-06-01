#ifndef PROTO_H
#define PROTO_H

#include <stdint.h>

void proto_send(int fd, const char* buffer, int32_t size);
int proto_recv(int fd, char* buffer);

#endif
