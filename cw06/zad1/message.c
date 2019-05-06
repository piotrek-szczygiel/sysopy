#include "message.h"

message_t new_message() {
  message_t message;
  message.type = TYPE_UNKNOWN;
  time(&message.timestamp);
  message.id = -1;
  strcpy(message.buffer, "");
  return message;
}
