#pragma once

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
    int fd_direct[2]; 
    int fd_back[2]; 
    char buf[BUF_SZ];
	
    int (*read_direct)();
    void (*read_back)();
    void (*write_back)();
    void (*write_direct)();
    int len;
} Pipe;
	
	
 int read_direct(Pipe *p);	
 void read_back(Pipe *p);
 void write_direct(Pipe *p);
 void write_back(Pipe *p);
 Pipe * constructPipe();
 
 
 
 
 
 