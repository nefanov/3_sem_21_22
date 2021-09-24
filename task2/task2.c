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

#define BUF_SZ 65536


typedef struct pPipe {
        int fd_direct[2]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
        int fd_back[2]; // array of r/w descriptors for "pipe()" call (for child-->parent direction)
    } Pipe;



    Pipe *constructPipe() {
        Pipe *p = malloc(sizeof(Pipe));
        pipe(p->fd_direct);
        pipe(p->fd_back);
        return p;
    }



int main(int argc, char *argv[]) {
    int p1[2], p2[2], status;
    long long int curr = 0;
    Pipe *p = constructPipe();
    FILE *fp_old;
    FILE *fp_new;
    char buf[BUF_SZ], b[BUF_SZ];
    if (argc == 1){
        printf("enter name of file as argument");
        return 1;
    }
    fp_old = fopen(argv[1], "rb");
    fp_new = fopen("result.txt", "wb");
    struct stat sb;
    stat(argv[1], &sb);
    pid_t child = fork();
    if (child == 0) {
        while (read(p->fd_direct[0], buf, BUF_SZ) != -1) {
            close(p->fd_direct[0]);
            write(p->fd_back[1], buf, BUF_SZ);
            close(p->fd_back[1]);              
        }
    }
    if (child > 0){
        int i = 0;
        while (fread(buf, sizeof(char), BUF_SZ, fp_old)) {            
            write(p->fd_direct[1], buf, BUF_SZ);
            close(p->fd_direct[1]);
            read(p->fd_back[0], buf, BUF_SZ);
            close(p->fd_back[0]);
            fwrite(buf, sizeof(char), BUF_SZ, fp_new);
            curr += strlen(buf)*256;
            if (i % 1000 == 0){
                system("clear");
                printf("%lld%%\n ", 100 * curr / sb.st_size);
            }
            ++i;
        }
    }
    system("clear");
    printf("done! file saved as result.txt");
    fclose(fp_old);
    fclose(fp_new);
    free(p);
    return 0;
}