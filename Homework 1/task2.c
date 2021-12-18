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

typedef struct pPipe {
	char buf[BUF_SZ];
    	int len;
    	int fd_back[2];
    	int fd_forw[2];
    	int (*re_forw)();
    	void (*wr_forw)();
    	void (*re_back)();
    	void (*wr_back)();
} Pipe;

int re_forw(Pipe* p) {
        read(p->fd_forw[0], &p->len, sizeof(int));
        int d = read(p->fd_forw[0], p->buf, p->len);
        close(p->fd_forw[0]);
        return d;
}

void wr_forw(Pipe* p) {
        write(p->fd_forw[1], &p->len, sizeof(int));
        write(p->fd_forw[1], p->buf, p->len);
        close(p->fd_forw[1]);
}

void re_back(Pipe* p) {
        read(p->fd_back[0], &p->len, sizeof(int));
        read(p->fd_back[0], p->buf, p->len);
        close(p->fd_back[0]);
}

void wr_back(Pipe* p) {
        write(p->fd_back[1], &p->len, sizeof(int));
        write(p->fd_back[1], p->buf, p->len);
        close(p->fd_back[1]);
}


Pipe* makePipe() {
	Pipe* p = malloc(sizeof(Pipe));
    	p->len = 0;
    	pipe(p->fd_forw);
    	pipe(p->fd_back);
    	p->re_forw = &re_forw;
    	p->wr_forw = &wr_forw;
    	p->re_back = &re_back;
    	p->wr_back = &wr_back;
	return p;
}

int main(int argc, char* argv[]) {
	int p1[2], p2[2], status;
	FILE* old_f;
	FILE* new_f;
	pid_t child = fork();
	Pipe* p = makePipe();
	if (argc < 2) {
		printf("Bad input\n");
	}
	old_f = fopen(argv[1], "rb");
	new_f = fopen("out", "wb");
	struct stat stat_buf;
	stat(argv[1], &stat_buf);
	if(child==0) {
		while (p->re_forw(p) != -1) {
			p->wr_back(p);
		}
	}
	else if (child > 0) {
	   	long long int count = 0;	
		while ((p->len = fread(p->buf, sizeof(char), BUF_SZ, old_f))) {
			p->wr_back(p);
			p->re_back(p);
			fwrite(p->buf, sizeof(char), p->len, new_f);
			if (count % 1000 == 0) {
				system("clear");
				printf("Working\n");
			}
			count++;
		}
		system("clear");
		fclose(old_f);
		fclose(new_f);
		free(p);
		printf("Done\n");
	}
	return 0;
}
