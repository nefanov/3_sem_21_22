#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SZ 254*256+3
#define PCG_SZ 256

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

size_t str_length(String *self) {
    return strlen(self->data);
}
void str_clear(String *self) {
    free(self->data);
}
String ctorString(int data_sz) {
    String Str;
    Str.data = calloc(data_sz, sizeof(char));

    // Methods
    Str.len = str_length;
    Str.clear = str_clear;

    return Str;
}

void p_snd(Pipe *s) {
    close(s->fd[0]);
    FILE *fin = fopen(s->fp_r, "r");

    int i = 1;
    size_t last_sym = BUF_SZ - 3;
    int num_pcg = 0;
    while(last_sym == BUF_SZ - 3) {
        s->buf.data[0] = (char)(i++);
        i %= 128;
        s->buf.data[1] = '0';
        s->buf.data[2] = '0';
        last_sym = fread(s->buf.data+3, sizeof(char), BUF_SZ - 3, fin);

        num_pcg = (int)(last_sym / PCG_SZ + 1);
        s->buf.data[1] = (char)num_pcg;
        s->buf.data[2] = 1;
        if (last_sym % PCG_SZ != 0) {
            s->buf.data[2] = (char)(last_sym % PCG_SZ + 1);
        }
        write(s->fd[1], s->buf.data, last_sym+3);
    }

    fclose(fin);
    close(s->fd[1]);
}
void p_rcv(Pipe *s) {
    close(s->fd[1]);
    FILE *fout = fopen(s->fp_w, "w");

    int j, i = 1;
    size_t length;
    unsigned char num_pcg, num_l_pcg;
    while(read(s->fd[0], s->buf.data, BUF_SZ) > 0) {
        j = (int)s->buf.data[0];
        if (j == i) {
            i++;
            i %= 128;
            num_pcg = (unsigned char)(s->buf.data[1] - 1);
            num_l_pcg = (unsigned char)(s->buf.data[2] - 1);
            length = num_pcg * PCG_SZ + num_l_pcg;
            fwrite(s->buf.data+3, sizeof(char), length, fout);
        }
    }

    fclose(fout);
    close(s->fd[0]);
}
void p_clear(Pipe *s) {
    s->buf.clear(&s->buf);
}
size_t p_size(Pipe *s) {
    return s->buf.len(&s->buf);
}
void p_pipe(Pipe *s) {
    if(pipe(s->fd) < 0) {
        perror("Invalid pipe");
        exit(1);
    }
}

Pipe ctorPipe(const char *fp_r, const char *fp_w) {
    Pipe Pipe;
    Pipe.buf = ctorString(BUF_SZ);
    Pipe.fp_r = fp_r;
    Pipe.fp_w = fp_w;

    Pipe.send = p_snd;
    Pipe.receive = p_rcv;
    Pipe.clear = p_clear;
    Pipe.size = p_size;
    Pipe.pipe = p_pipe;

    return Pipe;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Incorrect args");
    return 1;
  }


  Pipe pipe_PtoC = ctorPipe(argv[1], argv[2]);
  Pipe pipe_CtoP = ctorPipe(argv[2], argv[1]);

  pipe_PtoC.pipe(&pipe_PtoC);
  pipe_CtoP.pipe(&pipe_CtoP);

  switch (fork()) {
    case -1:
      perror("Incorrect fork");
      return 1;
    case 0:     /* Child process */
      
      pipe_PtoC.receive(&pipe_PtoC);
      pipe_CtoP.send(&pipe_CtoP);
      break;
      
    default:    /* Parent process */
      
      pipe_PtoC.send(&pipe_PtoC);
      
      pipe_CtoP.receive(&pipe_CtoP);
      
      break;
  }

  pipe_PtoC.clear(&pipe_PtoC);
  pipe_CtoP.clear(&pipe_CtoP);
  return 0;
}
