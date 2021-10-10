#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include "task2.h"


int main(int argc, char *argv[]) {
    int p1[2], p2[2], status;
    long long int curr = 0;
    int r;
    Pipe * p = constructPipe();
    FILE *fp_old;
    FILE *fp_new;
    if (argc != 2){
        printf("Check your arguments \n");
        return 1;
    }
    fp_old = fopen(argv[1], "rb");
    fp_new = fopen("result", "wb");
    struct stat sb;
    stat(argv[1], &sb);
    pid_t pid = fork();
    if (pid == 0) {
        while (p->read_direct(p) != -1) {
            p->write_back(p);             
        }
    }
    if (pid > 0){
        int i = 0;
        while ((p->len = fread(p->buf, sizeof(char), BUF_SZ, fp_old))) {        
            p->write_direct(p);
            p->read_back(p);
            fwrite(p->buf, sizeof(char), p->len, fp_new);
            curr += p->len;
            if (i % 1000 == 0){
                system("clear");
                printf("%lld%%\n ", 100 * curr / sb.st_size);
            }
            ++i;
        }
        system("clear");
        printf("File saved as result \n");
        fclose(fp_old);
        fclose(fp_new);
        free(p);
    }
    return 0;
} 
