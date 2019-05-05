#ifndef MESSAGE_H
#define MESSAGE_H

#include <string.h>
#include "types.h"

#define MESSAGE_BUFFER_SIZE 1024

typedef struct message_t {
  long type;
  int id;
  char buffer[MESSAGE_BUFFER_SIZE];
} message_t;

message_t new_message() {
  message_t message;
  message.type = TYPE_UNKNOWN;
  message.id = -1;
  strcpy(message.buffer, "");
  return message;
}

#define MESSAGE_SIZE sizeof(message_t) - sizeof(long)

#endif
