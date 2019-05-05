#ifndef MESSAGE_H
#define MESSAGE_H

#define MESSAGE_BUFFER_SIZE 1024

typedef struct message_t {
  long type;
  char buffer[MESSAGE_BUFFER_SIZE];
} message_t;

#define MESSAGE_SIZE sizeof(message_t) - sizeof(long)

#endif
