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

#define SZ 2048

int main(int argc, char* argv[]) {
	int size = SZ * sizeof(char);

	char* shared_addr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	int* shared_addr2 = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	int* shared_addr3 = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	for (int i = 0; i < atoi(argv[1]); i++) {
  		shared_addr2[0] = 0;
  		pid_t pid = fork();
  		if (pid) {
    		FILE *f;
    		f = fopen("file", "rb");
    		while ((shared_addr3[0] = fread(shared_addr, sizeof(char), SZ, f))) {
      			shared_addr2[0] = 1;
      			while (shared_addr2[0]) {
        			usleep(1);
      			}
    		}
    		shared_addr2[0] = 3;
    		fclose(f);
    		waitpid(pid, NULL, 0);
  		}

  		if (pid == 0) {
    		FILE *f;
    		f = fopen("res", "wb");
    		while (shared_addr2[0] != 3) {
      			if (shared_addr2[0]) {
       				fwrite(shared_addr, sizeof(char), shared_addr3[0], f);
       				shared_addr2[0] = 0;
     			} else {
     				usleep(1);
				}
   			}
   			fclose(f);
   		return 0;
 		}
	}
	munmap(shared_addr,size);
 	return 0;
}
