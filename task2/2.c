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

typedef struct pPipe Pipe;

typedef struct op_table Ops;

typedef struct op_table  {
    size_t (*rcv)(Pipe *self, pid_t child); 
    size_t (*snd)(Pipe *self, pid_t child); 
} Ops;

typedef struct pPipe {
        char data[BUF_SZ]; // intermediate buffer
        int fd_direct[2]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
        int fd_back[2]; // array of r/w descriptors for "pipe()" call (for child-->parent direction)
        int len; // data length in intermediate buffer
        Ops actions;
} Pipe;


size_t recieve(Pipe *p, pid_t c) {
	if (c == 0) {
		read(p->fd_direct[0], &p->len, sizeof(int));
		int f = read(p->fd_direct[0], p->data, p->len);
		close(p->fd_direct[0]);
		return f;
	} 
	if (c > 0) {
		read(p->fd_back[0], &p->len, sizeof(int));
		read(p->fd_back[0], p->data, p->len);
		close(p->fd_back[0]);
		
	}
	return -1;
}


size_t send(Pipe *p, pid_t c) {
	if (c > 0) {
		write(p->fd_direct[1], &p->len, sizeof(int));
		write(p->fd_direct[1], p->data, p->len);
		close(p->fd_direct[1]);
	}
	if (c == 0) {
		write(p->fd_back[1], &p->len, sizeof(int));
		write(p->fd_back[1], p->data, p->len);
		close(p->fd_back[1]);
	}
}

Pipe *constructPipe() {
	Pipe *p = malloc(sizeof(Pipe));
	p->data[0] = 0;
	p->len = 0;
	pipe(p->fd_direct);
	pipe(p->fd_back);
	p->actions.rcv= &recieve;
	p->actions.snd = &send;
	
	return p;
}
int main(int argc, char* argv[]) {
	Pipe *p = constructPipe();
	FILE* f0 = fopen(argv[1], "rb");
	FILE* f = fopen("out", "wb");
	pid_t pid = fork();
	if (pid == 0) {
		while (p->actions.rcv(p, pid) != -1) {
			p->actions.snd(p, pid);
		}
	}
	if (pid > 0) {
		while (p->len = fread(p->data, sizeof(char), BUF_SZ, f0)) {
			p->actions.snd(p, pid);
			p->actions.rcv(p, pid);
			fwrite(p->data, sizeof(char), p->len, f);
		}
	}
	fclose(f0);
	fclose(f);
	if (pid == 0)
		printf("Done\n");
	free(p);
	return 0;
}
