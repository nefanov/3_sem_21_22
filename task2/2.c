
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
        int fd_fw[2];
        int fd_bk[2];
        char buf[BUF_SZ];
        int (*read_fw)();
        void (*read_bk)();
        void (*write_fw)();
        void (*write_bk)();
        int len;
    } Pipe;

    int read_fw(Pipe *p) {
        read(p->fd_fw[0], &p->len, sizeof(int));
        int d = read(p->fd_fw[0], p->buf, p->len);
        close(p->fd_fw[0]);
        return d;
    }
    
    void read_bk(Pipe *p) {
        read(p->fd_bk[0], &p->len, sizeof(int));
        read(p->fd_bk[0], p->buf, p->len);
        close(p->fd_bk[0]);
    }
    
    void write_fw(Pipe *p) {
        write(p->fd_fw[1], &p->len, sizeof(int));
        write(p->fd_fw[1], p->buf, p->len);
        close(p->fd_fw[1]);
    }
    
    void write_bk(Pipe *p) {
        write(p->fd_bk[1], &p->len, sizeof(int));
        write(p->fd_bk[1], p->buf, p->len);
        close(p->fd_bk[1]);   
    }

    Pipe *createPipe() {
        Pipe *p = malloc(sizeof(Pipe));
        pipe(p->fd_fw);
        pipe(p->fd_bk);
        p->read_fw = &read_fw;
        p->read_bk = &read_bk;
        p->write_bk = &write_bk;
        p->write_fw = &write_fw;
        p->len = 0;
        return p;
    }

int main(int argc, char *argv[]) {
    int p1[2], p2[2], status;
    long long int curr = 0;
    Pipe *p = createPipe();
    FILE *f_old;
    FILE *f_new;
    if (argc < 2) {
        printf("Incorrect input.\n");
        return 1;
    }
    f_old = fopen(argv[1], "rb");
    f_new = fopen("result", "wb");
    struct stat st_buf;
    stat(argv[1], &st_buf);
    pid_t child = fork();
    if (child == 0) {
        while (p->read_fw(p) != -1) {
            p->write_bk(p);             
        }
    }
    else if (child > 0) {
        long long int i = 0;
        while ((p->len = fread(p->buf, sizeof(char), BUF_SZ, f_old))) {        
            p->write_bk(p);
            p->read_bk(p);
            fwrite(p->buf, sizeof(char), p->len, f_new);
            curr += p->len;
            if (i % 1000 == 0) {
                system("clear");
                printf("%lld%%\n ", 100 * curr / st_buf.st_size);
            }
            i++;
        }
        system("clear");
        fclose(f_old);
        fclose(f_new);
        free(p);
        printf("Check result in new file.\n");
    }
    return 0;
}
