#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>
#include <sys/fcntl.h>


//#define SZ 2048

#define QNAME "/example"

int main(int argc, char *argv[])
{
 mqd_t queue;
 struct mq_attr attr;
 unsigned prio = 1;
 attr.mq_flags = 0;
 attr.mq_msgsize = SZ;
 attr.mq_curmsgs = 0;
 char buf[SZ];
 int c; 

 for (int j = 0; j < atoi(argv[1]); ++j) {
  pid_t pid = fork();
  if (pid) {
    FILE *fp;
    fp = fopen("file", "rb");
    if ((queue = mq_open(QNAME, O_WRONLY, 0666, &attr)) == -1) {
      perror("Client: mq open error");
      return 1;
    }
    while ((c = fread(buf, sizeof(char), SZ, fp))) {
      if (mq_send(queue, c, sizeof(int), prio) == -1) {
        perror("Client: mq send error");
        return 1;
      }
      if (mq_send(queue, buf, c, prio) == -1) {
        perror("Client: mq send error");
        return 1;
      }
    }
    fclose(fp);
    waitpid(pid, NULL, 0);
  }

  if (pid == 0) {
    FILE *f;
    f = fopen("res", "wb");
    if ((server = mq_open(QNAME, O_RDONLY | O_CREAT, 0666, &attr)) == -1) {
      perror("Server: mq open failed");
      return 1;
    }
    c = 1
    while (c) {
      if (mq_receive(queue, c, sizeof(int), NULL) == -1) {
        perror ("Server: mq receive error");
        return 1;
      }
      if (c){
        if (mq_receive(queue, buf, c, NULL) == -1) {
          perror ("Server: mq receive error");
          return 1;
        }
        fwrite(buf, sizeof(char), c, f);
      }
    }
    fclose(f);
    return 0;
  }
}
return 0;
}