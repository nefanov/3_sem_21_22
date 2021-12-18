#include <stdio.h>
#include <unistd.h>
#include "src_pipe.h"
#include "src_main.h"


int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Incorrect args");
    return 1;
  }


  Pipe Pipe_P_C = ctorPipe(argv[1], argv[2]);
  Pipe Pipe_C_P = ctorPipe(argv[2], argv[1]);

  Pipe_P_C.pipe(&Pipe_P_C);
  Pipe_C_P.pipe(&Pipe_C_P);

  switch (fork()) {
    case -1:
      perror("Incorrect fork");
      return 1;
    case 0:     /* Child process */
      Pipe_P_C.receive(&Pipe_P_C);
      Pipe_C_P.send(&Pipe_C_P);
      break;
    default:    /* Parent process */
      Pipe_P_C.send(&Pipe_P_C);
      Pipe_C_P.receive(&Pipe_C_P);
      break;
  }

  Pipe_P_C.clear(&Pipe_P_C);
  Pipe_C_P.clear(&Pipe_C_P);
  return 0;
}