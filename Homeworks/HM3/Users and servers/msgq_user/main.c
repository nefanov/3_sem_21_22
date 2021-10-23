#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <string.h>
#include <limits.h>

#define MAX_MSGQ_SIZE 3096

typedef struct mymsg{
    long mtype;
    char data[MAX_MSGQ_SIZE];
}myMsg;

int main(int argc, char* const argv[])
{
    int msgq_id = msgget(10, 0666 | IPC_CREAT);

    if(msgq_id < 0)
    {
        perror("Can't open or create msgq");
        return -1;
    }

    int sourceFile = open(argv[1], 0666);
    char buf[MAX_MSGQ_SIZE];

    myMsg nextMessage;
    long bytesWritten = 0;
    long curBytesRead = 0;
    long curBytesWrite = 0;

    while ((curBytesRead = read(sourceFile, buf, MAX_MSGQ_SIZE - 1)) > 0) {
        buf[curBytesRead] = '\0';
        nextMessage.mtype = 1;
        strcpy(nextMessage.data, buf);

        curBytesWrite = msgsnd(msgq_id, (const void*)(&nextMessage), curBytesRead, 0);

        if(curBytesWrite < 0)
        {
            perror("Can't send message");
            return -1;
        }

        bytesWritten += curBytesRead;
    }

    nextMessage.mtype = 2;
    nextMessage.data[1] = '\0';
    msgsnd(msgq_id, (const void*)(&nextMessage), 1, 0);

    //printf("Send %ld bytes", bytesWritten);

    close(sourceFile);


    return 0;
}
