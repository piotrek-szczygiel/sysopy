#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "message.h"
#include "systemv_posix.h"
#include "types.h"
#include "utils.h"

int send(int queue, message_t* message) {
  return mq_send(queue, (char*)message, MESSAGE_WHOLE_SIZE, message->type);
}

int recv(int queue, message_t* message) {
  return mq_receive(queue, (char*)message, MESSAGE_WHOLE_SIZE, NULL);
}

int recv_nowait(int queue, message_t* message) {
  return recv(queue, message);
}

int set_nonblock(int queue) {
  struct mq_attr attr;
  attr.mq_flags = O_NONBLOCK;
  attr.mq_maxmsg = MAX_QUEUE_MESSAGES;
  attr.mq_msgsize = MESSAGE_WHOLE_SIZE;
  attr.mq_curmsgs = 0;

  return mq_setattr(queue, &attr, NULL);
}

int create_queue(int key) {
  char path[32];
  sprintf(path, "/%d", key);

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_QUEUE_MESSAGES;
  attr.mq_msgsize = MESSAGE_WHOLE_SIZE;
  attr.mq_curmsgs = 0;

  return mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
}

int open_queue(int key) {
  char path[32];
  sprintf(path, "/%d", key);

  return mq_open(path, O_WRONLY);
}

int close_queue(int queue) {
  return mq_close(queue);
}

int remove_queue(int queue, int key) {
  if (mq_close(queue) == -1)
    return -1;

  char path[32];
  sprintf(path, "/%d", key);

  return mq_unlink(path);
}
