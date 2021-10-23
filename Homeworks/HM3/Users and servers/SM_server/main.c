#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <semaphore.h>

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

    sem_t* mySem = sem_open(SEM_NAME, O_CREAT, 0666, 0);

    if(mySem == SEM_FAILED)
    {
        perror("Can't create semaphore");
        return -1;
    }

    sm_struct* smPointer = (sm_struct*)(shmat(shID, NULL, 0));
    smPointer->message_size = 0;
    long bytesRead = 0;

    sem_post(mySem);

    while (smPointer->message_size == 0);
    clock_t time1 = clock();

    bytesRead += smPointer->message_size;

    //write(1, smPointer->data, smPointer->message_size);

    smPointer->message_size = 0;

    while (1) {
        while (smPointer->message_size == 0);

        if(smPointer->message_size < 0)
            break;

        bytesRead += smPointer->message_size;

        //write(1, smPointer->data, smPointer->message_size);

        smPointer->message_size = 0;
    }

    clock_t time2 = clock();

    double time = ((double)(time2 - time1))/CLOCKS_PER_SEC;

    int fd = open(argv[1], O_RDWR | O_CREAT | O_APPEND, 0666);

    write(fd, (char*)(&time), 8);
    char bufy = ' ';
    write(fd, &bufy, 1);

    sem_close(mySem);
    shmdt((void*)(smPointer));
    close(fd);

    //printf("\nGot %ld bytes", bytesRead);

    return 0;
}
