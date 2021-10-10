#include "task2.h"


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


 Pipe * constructPipe() {
    Pipe * p = malloc(sizeof(Pipe));
    pipe(p->fd_direct);
    pipe(p->fd_back);
    p->read_direct = &read_direct;
    p->read_back = &read_back;
    p->write_back = &write_back;
    p->write_direct = &write_direct;
    p->len = 0;
    return p;
}
	
	
	
	