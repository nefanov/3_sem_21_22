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

    myMsg nextMessage;
    long bytesRead = 0;

    int msg_flag = msgrcv(msgq_id, &nextMessage, MAX_MSGQ_SIZE, 0, 0);

    if(msg_flag < 0)
    {
        perror("Error with receiving message");
        return -1;
    }

    clock_t time1 = clock();

    bytesRead += msg_flag;

    //write(1, nextMessage.data, msg_flag);

    while ((msg_flag = msgrcv(msgq_id, &nextMessage, MAX_MSGQ_SIZE, 0, 0)) > 0) {
        if(nextMessage.mtype == 2)
            break;

        bytesRead += msg_flag;

        //write(1, nextMessage.data, msg_flag);
    }

    clock_t time2 = clock();

    //printf("\n%ld bytes read", bytesRead);

    double time = ((double)(time2 - time1)) / CLOCKS_PER_SEC;

    int fd = open(argv[1], O_RDWR | O_CREAT | O_APPEND, 0666);

    write(fd, &time, 8);
    char buf = ' ';
    write(fd, &buf, 1);

    close(fd);
    msgctl(msgq_id, IPC_RMID, 0);

    return 0;
}
