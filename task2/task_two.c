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
    	int fd_forward[2];
    	int (*read_forward)();
    	void (*write_forward)();
    	void (*read_back)();
    	void (*write_back)();
} Pipe;

int read_forward(Pipe* p) {
        read(p->fd_forward[0], &p->len, sizeof(int));
        int data = read(p->fd_forward[0], p->buf, p->len);
        close(p->fd_forward[0]);
        return data;
}

void write_forward(Pipe* p) {
        write(p->fd_forward[1], &p->len, sizeof(int));
        write(p->fd_forward[1], p->buf, p->len);
        close(p->fd_forward[1]);
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
    	pipe(p->fd_forward);
    	pipe(p->fd_back);
    	p->read_forward = &read_forward;
    	p->write_forward = &write_forward;
    	p->read_back = &read_back;
    	p->write_back = &write_back;
	return p;
}

int main(int argc, char* argv[]) {
	int p1[2], p2[2], status;
	FILE* old_file;
	FILE* new_file;
	pid_t child = fork();
	Pipe* p = makePipe();
	if (argc < 2) {
		printf("Incorrect input, try again!\n");
	}
	old_file = fopen(argv[1], "rb");
	new_file = fopen("out", "wb");
	struct stat stat_buf;
	stat(argv[1], &stat_buf);
	if(child==0) {
		while (p->read_forward(p) != -1) {
			p->write_back(p);
		}
	}
	else if (child > 0) {
	   	long long int count = 0;	
		while ((p->len = fread(p->buf, sizeof(char), BUF_SZ, old_file))) {
			p->write_back(p);
			p->read_back(p);
			fwrite(p->buf, sizeof(char), p->len, new_file);
			if (count % 1000 == 0) {
				system("clear");
				printf("In process\n");
			}
			count++;
		}
		system("clear");
		fclose(old_file);
		fclose(new_file);
		free(p);
		printf("Success!\n");
	}
	return 0;
}

