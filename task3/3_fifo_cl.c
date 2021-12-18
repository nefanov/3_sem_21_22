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
size_t Write(int fout, const void *buf, size_t nbytes) {
  ssize_t written_sz = write(fout, buf, nbytes);
  return written_sz;
}

int main(int argc, char *argv[]) {
  printf("Client %d is running\n", getpid());
  char fifo_name[100];
  struct message msg;
  msg.clientpid = getpid();
  MakeFifoName(msg.clientpid, fifo_name, sizeof(fifo_name));
  int fd_server;
  fd_server = Open(SERVER_NAME, O_WRONLY);
  strcpy(msg.data, argv[1]);
  write(fd_server, &msg, sizeof(msg));
  int fd_client, fd_client_w;
  fd_client = Open(fifo_name, O_RDONLY);
  fd_client_w = Open(fifo_name, O_WRONLY);
  Read(fd_client, &msg, sizeof(msg));
  printf("Client %d: %s --> %s\n", getpid(), argv[1], msg.data);
  int fout = fileno(fopen(argv[2], "w"));
  int i = 1;
  size_t len;
  while (1) {
    Read(fd_client, msg.data, MSG_SIZE);
    len = (size_t)(msg.data[0]);
    if (len != MSG_SIZE) {
      if (len != 0) {
        Write(fout, msg.data + PROT_SZ, len);
      }
      break;
    } else {
      Write(fout, msg.data + PROT_SZ, len);
    }
  }

  close(fout);
  close(fd_server);
  close(fd_client_w);
  close(fd_client);
  unlink(fifo_name);
  printf("Client %d finished work.\n", getpid());
  return 0;
}
