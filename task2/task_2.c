#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
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
    	int fd_frw[2];
    	int (*read_frw)();
    	void (*write_frw)();
    	void (*read_back)();
    	void (*write_back)();
} Pipe;

int read_frw(Pipe* p) {
        read(p->fd_frw[0], &p->len, sizeof(int));
        int data = read(p->fd_frw[0], p->buf, p->len);
        close(p->fd_frw[0]);
        return data;
}

void write_frw(Pipe* p) {
        write(p->fd_frw[1], &p->len, sizeof(int));
        write(p->fd_frw[1], p->buf, p->len);
        close(p->fd_frw[1]);
}

void read_back(Pipe* p) {
        read(p->fd_back[0], &p->len, sizeof(int));
        read(p->fd_back[0], p->buf, p->len);
        close(p->fd_back[0]);
}

void write_back(Pipe* p) {
        write(p->fd_back[1], &p->len, sizeof(int));
        write(p->fd_back[1], p->buf, p->len);
        close(p->fd_back[1]);
}


Pipe* makePipe() {
	Pipe* p = malloc(sizeof(Pipe));
    	p->len = 0;
    	pipe(p->fd_frw);
    	pipe(p->fd_back);
    	p->read_frw = &read_frw;
    	p->write_frw = &write_frw;
    	p->read_back = &read_back;
    	p->write_back = &write_back;
	return p;
}

int main(int argc, char* argv[]) {
	int p1[2], p2[2], status;
	FILE* f_old;
	FILE* f_new;
	pid_t child = fork();
	Pipe* p = makePipe();
	if (argc < 2) {
		printf("incorrect input\n");
	}
	f_old = fopen(argv[1], "rb");
	f_new = fopen("out", "wb");
	struct stat stat_buf;
	stat(argv[1], &stat_buf);
	if(child==0) {
		while (p->read_frw(p) != -1) {
			p->write_back(p);
		}
	}
	else if (child > 0) {
	   	long long int count = 0;	
		while ((p->len = fread(p->buf, sizeof(char), BUF_SZ, f_old))) {
			p->write_back(p);
			p->read_back(p);
			fwrite(p->buf, sizeof(char), p->len, f_new);
			if (count % 1000 == 0) {
				system("clear");
				printf("in process\n");
			}
			count++;
		}
		system("clear");
		fclose(f_old);
		fclose(f_new);
		free(p);
		printf("file saved\n");
	}
	return 0;
}