#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <string.h>

#define MSG_TEXT_SZ 128
#define STD_MSG_TYPE 1
#define LAST_MSG_TYPE 255

typedef struct msg {
  long mtype;
  char mtext[MSG_TEXT_SZ];
} Message;

int get_msg_id(const char *path);
void send(int msg_id, char *argv[]);
void receive(int msg_id, char *argv[]);