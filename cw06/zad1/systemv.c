#include <sys/msg.h>
#include "error.h"
#include "message.h"
#include "systemv_posix.h"
#include "types.h"
#include "utils.h"

int send(int queue, message_t* message) {
  return msgsnd(queue, message, MESSAGE_SIZE, 0);
}

int recv(int queue, message_t* message) {
  return msgrcv(queue, message, MESSAGE_SIZE, -TYPE_LAST, 0);
}

int recv_nowait(int queue, message_t* message) {
  return msgrcv(queue, message, MESSAGE_SIZE, -TYPE_LAST, IPC_NOWAIT);
}

int set_nonblock(int queue) {
  return 0;
}

int create_queue(int key) {
  return msgget(key, IPC_CREAT | IPC_EXCL | 0600);
}

int open_queue(int key) {
  return msgget(key, 0);
}

int remove_queue(int queue, int key) {
  return msgctl(queue, IPC_RMID, NULL);
}
