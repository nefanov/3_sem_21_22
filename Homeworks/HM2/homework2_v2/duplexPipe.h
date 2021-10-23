#ifndef DUPLEXPIPE_H
#define DUPLEXPIPE_H

#define NUM_OF_BYTES_READING_ONCE 1000
#define INTERMEDIATE_BUF_SIZE 5000

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct duplexPipe dPipe;
typedef struct opsTable Ops;

typedef struct opsTable{
    size_t (*receiveData)(dPipe* this);
    size_t (*sendData)(dPipe* this, _Bool flag);
    void (*destructor)(dPipe* this);
}Ops;

typedef struct duplexPipe
{
    char* filename;
    int fdDirect[2]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
    int fdBack[2]; // array of r/w descriptors for "pipe()" call (for child-->parent direction)
    Ops actions;

    pid_t parentPid;
    _Bool coordinated;
}dPipe;

size_t sendDataChild(dPipe* this, _Bool flag);              //flag is 1, if we send from file; flag is 0 if send from pipe
size_t receiveDataChild(dPipe* this);
size_t receiveDataParent(dPipe* this);
size_t sendDataParent(dPipe* this, _Bool flag);
_Bool initPipe(dPipe* this, char* const filename);
void destructorPipe(dPipe* this);

#endif // DUPLEXPIPE_H
