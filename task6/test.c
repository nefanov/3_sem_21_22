#include <stdio.h>
#include <sys/mman.h>

int main(){
    while (1) {
        int *ptr = mmap ( NULL, 5*sizeof(int), 
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
        int err = munmap(ptr, 10*sizeof(int));
    }
return 0;
}