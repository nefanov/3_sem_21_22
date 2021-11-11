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

#define BUF_SZ 65532

typedef struct pPipe Pipe;

typedef struct pPipe {
        int fd_direct[2]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
        int fd_back[2]; // array of r/w descriptors for "pipe()" call (for child-->parent direction)
        char buf[BUF_SZ];
        int (*read_direct)();
        void (*read_back)();
        void (*write_back)();
        void (*write_direct)();
        int len;
    } Pipe;


    int read_direct(Pipe *p) {
        read(p->fd_direct[0], &p->len, sizeof(int));
        int r = read(p->fd_direct[0], p->buf, p->len);
        close(p->fd_direct[0]);
        return r;
    }
    void read_back(Pipe *p) {
        read(p->fd_back[0], &p->len, sizeof(int));
        read(p->fd_back[0], p->buf, p->len);
        close(p->fd_back[0]);
    }
    void write_direct(Pipe *p){
        write(p->fd_direct[1], &p->len, sizeof(int));
        write(p->fd_direct[1], p->buf, p->len);
        close(p->fd_direct[1]);
    }
    void write_back(Pipe *p){
        write(p->fd_back[1], &p->len, sizeof(int));
        write(p->fd_back[1], p->buf, p->len);
        close(p->fd_back[1]);   
    }


    Pipe *constructPipe() {
        Pipe *p = malloc(sizeof(Pipe));
        pipe(p->fd_direct);
        pipe(p->fd_back);
        p->read_direct = &read_direct;
        p->read_back = &read_back;
        p->write_back = &write_back;
        p->write_direct = &write_direct;
        p->len = 0;
        return p;
    }



    int main(int argc, char *argv[]) {
        int p1[2], p2[2], status;
        long long int curr = 0;
        int r;
        Pipe *p = constructPipe();
        FILE *fp_old;
        FILE *fp_new;
        if (argc == 1){
            printf("enter name of file as an argument");
            return 1;
        }
        fp_old = fopen(argv[1], "rb");
        fp_new = fopen("result", "wb");
        struct stat sb;
        stat(argv[1], &sb);
        pid_t child = fork();
        if (child == 0) {
            while (p->read_direct(p) != -1) {
                p->write_back(p);             
            }
        }
        if (child > 0){
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
            printf("done! file saved as result");
            fclose(fp_old);
            fclose(fp_new);
            free(p);
        }
        return 0;
    }
