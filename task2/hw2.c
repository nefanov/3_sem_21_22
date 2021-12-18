#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define buf_size 65322

typedef struct pPipe Pipe;
typedef struct op_table Ops;

typedef struct op_table {
  size_t( * rcv)(Pipe * self);
  size_t( * snd)(Pipe * self);
}
Ops;

typedef struct pPipe {
  int fd_direct[2];
  int fd_back[2];
  char buf[buf_size];
  void( * write_direct)();
  void( * write_back)();
  int( * read_direct)();
  void( * read_back)();
  int len;
}
Pipe;

void write_direct(Pipe * p) {
  write(p -> fd_direct[1], & p -> len, sizeof(int));
  write(p -> fd_direct[1], p -> buf, p -> len);
  close(p -> fd_direct[1]);
}
void write_back(Pipe * p) {
  write(p -> fd_back[1], & p -> len, sizeof(int));
  write(p -> fd_back[1], p -> buf, p -> len);
  close(p -> fd_back[1]);
}
int read_direct(Pipe * p) {
  read(p -> fd_direct[0], & p -> len, sizeof(int));
  int r = read(p -> fd_direct[0], p -> buf, p -> len);
  close(p -> fd_direct[0]);
  return r;
}
void read_back(Pipe * p) {
  read(p -> fd_back[0], & p -> len, sizeof(int));
  read(p -> fd_back[0], p -> buf, p -> len);
  close(p -> fd_back[0]);
}

Pipe * constructPipe() {
  Pipe * p = malloc(sizeof(Pipe));
  pipe(p -> fd_direct);
  pipe(p -> fd_back);
  p -> write_direct = & write_direct;
  p -> write_back = & write_back;
  p -> read_direct = & read_direct;
  p -> read_back = & read_back;
  p -> len = 0;
  return p;
}

int main(int argc, char * argv[]) {
  int p1[2], p2[2];
  long long int now = 0;
  Pipe * p = constructPipe();
  FILE * fp_old;
  FILE * fp_new;
  fp_old = fopen("file.txt", "r");
  fp_new = fopen("result.txt", "w");

  pid_t child = fork();
  if (child == 0) {
    while (p -> read_direct(p) != -1) {
      p -> write_back(p);
    }
  }

  if (child != 0) {
    int i = 0;
    while ((p -> len = fread(p -> buf, sizeof(char), buf_size, fp_old))) {
      p -> write_direct(p);
      p -> read_back(p);
      fwrite(p -> buf, sizeof(char), p -> len, fp_new);
      now += p -> len;
      ++i;
    }
    printf("success: the file has been saved as result");
    fclose(fp_old);
    fclose(fp_new);
    free(p);
  }

  return 0;
}
