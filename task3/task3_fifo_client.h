#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define SERVER_FIFO_NAME "../channels/fifo_server"
#define MSG_DATA_SZ 128
#define READY_MSG "Ready"
#define CHANNELS_REL_PATH "../channels/"
#define PROT_SZ 1
#define MAX_READ_SZ (MSG_DATA_SZ - PROT_SZ)

struct easy_message {
  pid_t sm_clientpid;
  char sm_data[MSG_DATA_SZ];
};

int create_Fifo_Name(pid_t pid, char *name, size_t name_max);
int Open(const char *path, int flag);
void Read(int fd, void *buf, size_t nbytes);
size_t Write(int fout, const void *buf, size_t nbytes);

