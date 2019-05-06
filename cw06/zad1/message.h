#ifndef MESSAGE_H
#define MESSAGE_H

#include <string.h>
#include "types.h"

#define MAX_QUEUE_MESSAGES 16
#define MESSAGE_WHOLE_SIZE sizeof(message_t)

#define MESSAGE_BUFFER_SIZE 1024

#define MESSAGE_SIZE sizeof(message_t) - sizeof(long)

typedef struct message_t {
  long type;
  int id;
  char buffer[MESSAGE_BUFFER_SIZE];
} message_t;

message_t new_message();

#endif
