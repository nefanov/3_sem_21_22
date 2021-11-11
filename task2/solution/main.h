#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PCG_SZ 256
#define BUF_SZ 254*256+3

typedef struct pString String;
typedef struct pString {
  char *data;

  // Methods:
  size_t (*len)(String *self);
  void (*clear)(String *self);
} String;

typedef struct pPipe Pipe;
typedef struct pPipe {
  int fd[2];
  String buf;
  const char *fp_r;
  const char *fp_w;

  // Methods:
  void (*send)(Pipe *self);
  void (*receive)(Pipe *self);
  void (*clear)(Pipe *self);
  size_t (*size)(Pipe *self);
  void (*pipe)(Pipe *self);
} Pipe;

// String methods
size_t str_length(String *self);
void str_clear(String *self);

String ctorString(int data_sz);
Pipe ctorPipe(const char *fp_w, const char *fp_r);
void p_snd(Pipe *self);
void p_rcv(Pipe *self);
void p_clear(Pipe *self);
size_t p_size(Pipe *self);
void p_pipe(Pipe *self);

