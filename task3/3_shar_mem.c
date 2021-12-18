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
#include <sys/mman.h>
#include <fcntl.h>
#define BUF 2048
int main(int argc, char* argv[]) {
	int len = BUF * sizeof(char);
	char* sa_1 = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	int* sa_2 = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	int* sa_3 = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	for (int i = 0; i < atoi(argv[1]); i++) {
		sa_2[0] = 0;
		pid_t pid = fork();
		if (pid) {
			FILE *f;
			f = fopen("in", "r");
			while ((sa_3[0] = fread(sa_1, sizeof(char), BUF, f))) {
				sa_2[0] = 1;
				while (sa_2[0]) {
					usleep(1);
				}
			}
			sa_2[0] = 5;
			fclose(f);
			waitpid(pid, NULL, 0);
		}

		if (pid == 0) {
			FILE *f;
			f = fopen("out", "w");
			while (sa_2[0] != 5) {
				if (sa_2[0]) {
					fwrite(sa_1, sizeof(char), sa_3[0], f);
					sa_2[0] = 0;
				}
				else {
					usleep(1);
				}
			}
			fclose(f);
			return 0;
		}
	}
	munmap(sa_1, len);
	return 0;
}
