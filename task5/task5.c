#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>

static pid_t pid;

static int byte = 0;
static int bits = 128;


// Nothing
void empty(int signo) {
}

// SIGUSR1
void GotOne(int signo) {
  byte += bits;
  bits /= 2;
  kill(pid, SIGUSR1);
}

// SIGUSR2
void GotZero(int signo) {
  bits /= 2;
  kill(pid, SIGUSR1);
}

int main(int argc, char *argv[]) {
  pid_t ppid = getpid();

  sigset_t set;

  // SIGCHLD -> exit
  struct sigaction act_exit;
  memset(&act_exit, 0, sizeof(act_exit));
  act_exit.sa_handler = exit;
  sigfillset(&act_exit.sa_mask);
  sigaction(SIGCHLD, &act_exit, NULL);

  // SIGUSR1 -> GotOne()
  struct sigaction act_one;
  memset(&act_one, 0, sizeof(act_one));
  act_one.sa_handler = GotOne;
  sigfillset(&act_one.sa_mask);
  sigaction(SIGUSR1, &act_one, NULL);

  // SIGUSR2 -> GotZero()
  struct sigaction act_zero;
  memset(&act_zero, 0, sizeof(act_zero));
  act_zero.sa_handler = GotZero;
  sigfillset(&act_zero.sa_mask);
  sigaction(SIGUSR2, &act_zero, NULL);

  //инициализируем сигналы
  sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGUSR2);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, NULL);
  sigemptyset(&set);

  pid = fork();

  //Передатчик
  if (pid == 0) {
    int fd;
    char c;

    // SIGUSR1 - empty()
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
      // SIGALRM будет получен, если не успеет ответить за секунду
      for (int i = 128; i >= 1; i /= 2){
        if (i & c)              // 1
          kill(ppid, SIGUSR1);
        else                      // 0
          kill(ppid, SIGUSR2);
      //приостанавливаем до получения сигнала
      sigsuspend(&set);
  }
}
    // Файл кончился
exit(EXIT_SUCCESS);
}


// Получаем пока передатчик не умрёт
FILE *f = fopen(argv[2], "w");
do {
    if (bits == 0) {
      write(fileno(f), &byte, 1);
      bits = 128;
      byte = 0;
    }
    sigsuspend(&set); // Ждём сигнал от передатчика
} while (1);
}