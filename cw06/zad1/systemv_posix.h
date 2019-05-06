#ifndef SYSTEMV_H
#define SYSTEMV_H

#include "message.h"

int send(int queue, message_t* message);
int recv(int queue, message_t* message);
int recv_nowait(int queue, message_t* message);

int create_queue(int key);
int open_queue(int key);
int remove_queue(int queue);

#endif
