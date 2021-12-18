#ifndef DUPLEXPIPE_H
#define DUPLEXPIPE_H

#define PARENT_TEMP_FILE_NAME "parentTemerateBuffer.txt"
#define CHILD_TEMP_FILE_NAME "childTemerateBuffer.txt"
#define BUF_CAPACITY 4096

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct duplexPipe dPipe;

typedef struct OperationalTabel{
    size_t (*receiveData)(dPipe*);
    size_t (*sendData)(dPipe*, char*);
}Opt;

typedef struct duplexPipe{
    int fdDirect[2];
    int fdBack[2];
    Opt operations;

    short parOrChild;           //if parent then 1
}dPipe;

size_t receiveData(dPipe* self);
size_t sendData(dPipe* self, char* sourceFile);

int init_dPipe(dPipe* p);
int child_Pipe_intit(dPipe* p);
int parent_Pipe_intit(dPipe* p);

void child_Pipe_terminate(dPipe* p);
void parent_Pipe_terminate(dPipe* p);

#endif // DUPLEXPIPE_H
