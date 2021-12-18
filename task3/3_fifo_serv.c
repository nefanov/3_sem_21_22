#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>

#define MSG_SIZE 128
#define PROT_SZ 1
#define CHANNELS_REL_PATH "../channels/"
#define SERVER_NAME "../channels/fifo_server"

struct message {
    pid_t clientpid;
    char data[MSG_SIZE];
};

char fifo_name[100] = "\0";

int MakeFifoName(pid_t pid, char *name, size_t name_max) {
  snprintf(name, name_max, "%sfifo%d", CHANNELS_REL_PATH, pid);
  return 0;
}
int Open(const char *path, int flag) {
  int fd = open(path, flag);
  return fd;
}
void Read(int fd, void *buf, size_t nbytes) {
  ssize_t nread = read(fd, buf, nbytes);
}

int GetStarted(int fd_server, struct message *msg) {
  int fd_client;
  Read(fd_server, msg, sizeof(*msg));
  printf("Msg received: %s\nProcessing..\n", msg->data);
  MakeFifoName(msg->clientpid, fifo_name, sizeof(fifo_name));
  printf("fifo_name: %s\n", fifo_name);

  struct message ready_msg;
  fd_client = Open(fifo_name, O_WRONLY);
  strcpy(ready_msg.data, "Ready");
  write(fd_client, &ready_msg, sizeof(ready_msg));
  return fd_client;
}

int main() {
  mkfifo(SERVER_NAME, 0666);
  printf("Server is running\n");
  int fd_server, fd_server_w;
  fd_server = Open(SERVER_NAME, O_RDONLY);
  fd_server_w = Open(SERVER_NAME, O_WRONLY);
  struct message msg;
  int fd_client = GetStarted(fd_server, &msg);
  printf("Try to open source file..\n");
  int fin = Open(msg.data, O_RDONLY);
  printf("Server is starting to send data from the file\n");
  size_t read_sz;
  int i = 1;
  while (1) {
    read_sz = read(fin, msg.data+PROT_SZ, MSG_SIZE);
    if (read_sz != MSG_SIZE) {
      if (read_sz != 0) {
        msg.data[0] = (char)read_sz;
        write(fd_client, msg.data, read_sz + PROT_SZ);
      }
      printf("Server: EOF!\n");
      break;
    } else {
      msg.data[0] = (char)read_sz;
      write(fd_client, msg.data, read_sz + PROT_SZ);
    }
  }

  close(fin);
  close(fd_client);
  close(fd_server);

  printf("Server finished work.\n");

  return 0;
}
