#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <math.h>

static pid_t pid;

static int byte = 0;
static int st = 7;



void empty(int signo) {
}


void GotOne(int signo) {
  byte += pow(2, st);
  st--;
  kill(pid, SIGUSR1);
}


void GotZero(int signo) {
  st--;
  kill(pid, SIGUSR1);
}

int main(int argc, char *argv[]) {
  pid_t ppid = getpid();

  sigset_t set;

  
  struct sigaction act_exit;
  memset(&act_exit, 0, sizeof(act_exit));
  act_exit.sa_handler = exit;
  sigfillset(&act_exit.sa_mask);
  sigaction(SIGCHLD, &act_exit, NULL);

  
  struct sigaction act_one;
  memset(&act_one, 0, sizeof(act_one));
  act_one.sa_handler = GotOne;
  sigfillset(&act_one.sa_mask);
  sigaction(SIGUSR1, &act_one, NULL);

  
  struct sigaction act_zero;
  memset(&act_zero, 0, sizeof(act_zero));
  act_zero.sa_handler = GotZero;
  sigfillset(&act_zero.sa_mask);
  sigaction(SIGUSR2, &act_zero, NULL);

  
  sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGUSR2);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, NULL);
  sigemptyset(&set);

  pid = fork();


  if (pid == 0) {
    int fd;
    char c;

    struct sigaction act_empty;
    memset(&act_empty, 0, sizeof(act_empty));
    act_empty.sa_handler = empty;
    sigfillset(&act_empty.sa_mask);
    sigaction(SIGUSR1, &act_empty, NULL);

    if ((fd = open(argv[1], O_RDONLY)) < 0 ){
      perror("some troubles with opening file occured");
      exit(EXIT_FAILURE);
  }

  while (read(fd, &c, 1) > 0){
      for (int i = 128; i >= 1; i /= 2){
        if (i & c)              // 1
          kill(ppid, SIGUSR1);
        else                      // 0
          kill(ppid, SIGUSR2);
      sigsuspend(&set);
  }
}
    exit(EXIT_SUCCESS);
}



FILE *f = fopen(argv[2], "w");
do {
    if (st < 0) {
      write(fileno(f), &byte, 1);
      st = 7;
      byte = 0;
    }
    sigsuspend(&set); 
} while (1);
} 
