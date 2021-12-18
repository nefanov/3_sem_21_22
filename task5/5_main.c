#include "5_main.h"

static int out_char = 0;
static int counter = 128;


void child_exit(int signo) {
  exit(EXIT_SUCCESS);
}

void parent_exit(int signo) {
  exit(EXIT_SUCCESS);
}

void empty(int signo) {
}

void got_1(int signo) {
  out_char += counter;
  counter /= 2;
  kill(pid, SIGUSR1);
}

void got_0(int signo) {
  counter /= 2;
  kill(pid, SIGUSR1);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Use: %s [source] [destination]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // pid родителя
  pid_t ppid = getpid();

  sigset_t set;

  struct sigaction act_exit;
  memset(&act_exit, 0, sizeof(act_exit));
  act_exit.sa_handler = child_exit;
  sigfillset(&act_exit.sa_mask);
  sigaction(SIGCHLD, &act_exit, NULL);

  struct sigaction act_one;
  memset(&act_one, 0, sizeof(act_one));
  act_one.sa_handler = got_1;
  sigfillset(&act_one.sa_mask);
  sigaction(SIGUSR1, &act_one, NULL);

  struct sigaction act_zero;
  memset(&act_zero, 0, sizeof(act_zero));
  act_zero.sa_handler = got_0;
  sigfillset(&act_zero.sa_mask);
  sigaction(SIGUSR2, &act_zero, NULL);
  
  sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGUSR2);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, NULL);
  sigemptyset(&set);

  pid = fork();

  // Ребёнок 
  if (pid == 0) {
    int fd = 0;
    char c = 0;
    sigemptyset(&set);
    
    struct sigaction act_empty;
    memset(&act_empty, 0, sizeof(act_empty));
    act_empty.sa_handler = empty;
    sigfillset(&act_empty.sa_mask);
    sigaction(SIGUSR1, &act_empty, NULL);
    
    struct sigaction act_alarm;
    memset(&act_alarm, 0, sizeof(act_alarm));
    act_alarm.sa_handler = parent_exit;
    sigfillset(&act_alarm.sa_mask);
    sigaction(SIGALRM, &act_alarm, NULL);

    if ((fd = open(argv[1], O_RDONLY)) < 0 ){
      perror("Can't open file");
      exit(EXIT_FAILURE);
    }

    int i;

    while (read(fd, &c, 1) > 0){
      // SIGALRM получен если родитель не отвечает за секунду
      alarm(1);
      
      for ( i = 128; i >= 1; i /= 2){
        if ( i & c )              
          kill(ppid, SIGUSR1);
        else                      
          kill(ppid, SIGUSR2);
        
        sigsuspend(&set);
      }
    }

    exit(EXIT_SUCCESS);
  }

  errno = 0;
  
  FILE *fout = fopen(argv[2], "w");
  do {
    if(counter == 0) {

      write(fileno(fout), &out_char, 1);  
      fflush(fout);
      counter=128;
      out_char = 0;
    }
    sigsuspend(&set); 
  } while (1);

  exit(EXIT_SUCCESS);
}