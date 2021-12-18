#pragma once
#include <stdio.h>
#include "string_src.h"

#define PCG_SZ 256

/// Тут описываем класс Пайп и методы

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

// Pipe methods
void p_snd(Pipe *self); // size_t ???
void p_rcv(Pipe *self); // size_t ???
void p_clear(Pipe *self);
size_t p_size(Pipe *self);
void p_pipe(Pipe *self);

Pipe ctorPipe(const char *fp_w, const char *fp_r);
