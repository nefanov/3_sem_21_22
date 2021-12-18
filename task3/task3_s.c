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

//#define SZ 2048

int main(int argc, char *argv[])
{
 int size = SZ * sizeof(char);
 int size2 = sizeof(int);
 char *addr =  mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
 void *addr2 = mmap(0, size2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
 void *addr3 = mmap(0, size2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

 char *shared = addr;
 int *shared2 = addr2;
 int *shared3 = addr3;

 for (int j = 0; j < atoi(argv[1]); ++j) {
  shared2[0] = 0;
  pid_t pid = fork();
  if (pid) {
    FILE *fp;
    fp = fopen("file", "rb");
    while ((shared3[0] = fread(shared, sizeof(char), SZ, fp))) {
      shared2[0] = 1;
      while (shared2[0]) {
        usleep(1);
      }
    }
    shared2[0] = 3;
    fclose(fp);
    waitpid(pid, NULL, 0);
  }

  if (pid == 0) {
    FILE *f;
    f = fopen("res", "wb");
    while (shared2[0] != 3) {
      if (shared2[0]) {
       fwrite(shared, sizeof(char), shared3[0], f);
       shared2[0] = 0;
     } else
     usleep(1);
   }
   fclose(f);
   return 0;
 }
}
 munmap(addr,size);
 return 0;
}