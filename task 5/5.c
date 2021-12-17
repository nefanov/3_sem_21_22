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

void get_zero(int signo) {
	bits /= 2;
	kill(pid, SIGUSR1);
}

void get_one(int signo) {
	byte += bits;
	bits /= 2;
	kill(pid, SIGUSR1);
}

void empty(int signo) {
}

int main(int argc, char* argv[]) {
	sigset_t set;
	pid_t ppid = getpid();
	struct sigaction ex;
	struct sigaction one;
	struct sigaction zero;
  	memset(&ex, 0, sizeof(ex));
  	ex.sa_handler = exit;
  	sigfillset(&ex.sa_mask);
  	sigaction(SIGCHLD, &ex, NULL);
	memset(&one, 0, sizeof(one));
    	one.sa_handler = get_one;
    	sigfillset(&one.sa_mask);
    	sigaction(SIGUSR1, &one, NULL);
	memset(&zero, 0, sizeof(zero));
    	zero.sa_handler = get_zero;
    	sigfillset(&zero.sa_mask);
    	sigaction(SIGUSR2, &zero, NULL);
	pid = fork();
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &set, NULL);
	sigemptyset(&set);
	if (pid == 0) {
		int fd;
		char a;
		struct sigaction emp;
		memset(&emp, 0, sizeof(emp));
    		emp.sa_handler = empty;
    		sigfillset(&emp.sa_mask);
    		sigaction(SIGUSR1, &emp, NULL);
		fd = open(argv[1], O_RDONLY);
		while (read(fd, &a, 1) > 0) {
			for (int i = 128; i > 0; i /= 2) {
				if (i & a) {
					kill(ppid, SIGUSR1);
				}
				else {
					kill(ppid, SIGUSR2);
				}
				sigsuspend(&set);
			}
		}
		exit(EXIT_SUCCESS);
	}
	FILE* file = fopen(argv[2], "w");
	do {
		if (bits == 0) {
			write(fileno(file), &byte, 1);
			byte = 0;
			bits = 128;
		}
		sigsuspend(&set);
	} while(1);
	exit(EXIT_SUCCESS);
}
