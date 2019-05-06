#ifndef SYSTEMV_H
#define SYSTEMV_H

#include "message.h"

int send(int queue, message_t* message);
int recv(int queue, message_t* message);
int recv_nowait(int queue, message_t* message);
int set_nonblock(int queue);

int create_queue(int key);
int open_queue(int key);
int close_queue(int queue);
int remove_queue(int queue, int key);

#endif
