#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <string.h>

#define SEM_NAME "semaphoremy"
#define SM_SEGMENT_SIZE 5144

typedef struct SM_struct {
    int message_size;
    char data[5140];
}sm_struct;

int main(int argc, char* const argv[])
{
    int shID = shmget(9, SM_SEGMENT_SIZE, 0666 | IPC_CREAT);

    if(shID < 0)
    {
        perror("Can't create shared memory");
        return -1;
    }

    int sourceFile = open(argv[1], O_RDWR);
    char buf[SM_SEGMENT_SIZE];

    sem_t* mySem = sem_open(SEM_NAME, O_CREAT, 0666, 0);

    sm_struct* smPointer = (sm_struct*)(shmat(shID, NULL, 0));

    long bytesWritten = 0;
    long bytesCurrentRead = 0;

    sem_wait(mySem);

    bytesCurrentRead = read(sourceFile, buf, SM_SEGMENT_SIZE - 1);

    if(bytesCurrentRead <= 0)
    {
        return 0;
    }
    bytesWritten += bytesCurrentRead;
    buf[bytesCurrentRead] = '\0';

    strcpy(smPointer->data, buf);
    smPointer->message_size = bytesCurrentRead;

    while (1) {
        while (smPointer->message_size == bytesCurrentRead);

        if((bytesCurrentRead = read(sourceFile, buf, SM_SEGMENT_SIZE - 1)) <= 0)
        {
            smPointer->message_size = -1;
            break;
        }

        bytesWritten += bytesCurrentRead;
        buf[bytesCurrentRead] = '\0';
        strcpy(smPointer->data, buf);
        smPointer->message_size = bytesCurrentRead;
    }

    //printf("%ld bytes were sent", bytesWritten);


    close(sourceFile);
    sem_close(mySem);
    shmdt((void*)(smPointer));

    return 0;
}
