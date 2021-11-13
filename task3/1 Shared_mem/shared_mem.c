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

#define SIZE 2048

int main(int argc, char* argv[]) {
	int size = SIZE * sizeof(char);

	char* shared_address_1 = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	int* shared_address_2 = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	int* shared_address_3 = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	for (int i = 0; i < atoi(argv[1]); i++) {
		shared_address_2[0] = 0;
		pid_t pid = fork();
		if (pid) {
			FILE *f;
			f = fopen("file", "rb");
			while ((shared_address_3[0] = fread(shared_address_1, sizeof(char), SIZE, f))) {
				shared_address_2[0] = 1;
				while (shared_address_2[0]) {
					usleep(1);
				}
			}
			shared_address_2[0] = 3;
			fclose(f);
			waitpid(pid, NULL, 0);
		}

		if (pid == 0) {
			FILE *f;
			f = fopen("res", "wb");
			while (shared_address_2[0] != 3) {
				if (shared_address_2[0]) {
					fwrite(shared_address_1, sizeof(char), shared_address_3[0], f);
					shared_address_2[0] = 0;
				}
				else {
					usleep(1);
				}
			}
			fclose(f);
			return 0;
		}
	}
	munmap(shared_address_1, size);
	return 0;
	}
