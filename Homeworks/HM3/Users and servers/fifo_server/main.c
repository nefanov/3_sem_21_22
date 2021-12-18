#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define FIFO_NAME "fifo_exercise"

int main(int argc, char* const argv[])
{
    int fifo_create_flag = mkfifo(FIFO_NAME, 0666);

    int fifo_descriptor = open(FIFO_NAME, O_RDONLY);

    if(fifo_descriptor < 0)
    {
        perror("Can't open fifo");
        return -1;
    }

    char buf[PIPE_BUF];                                 //temporary buf

    long curBytesRead = 0;
    long bytesRead = 0;

    curBytesRead = read(fifo_descriptor, buf, PIPE_BUF);
    clock_t time1 = clock();                                    //start counting time

    while (curBytesRead > 0) {
        //write(1, buf, curBytesRead);

        bytesRead += curBytesRead;

        curBytesRead = read(fifo_descriptor, buf, PIPE_BUF);
    }

    clock_t time2 = clock();                                    //end counting time

    double program_time = ((double)(time2 - time1) / CLOCKS_PER_SEC);

    int fd = open(argv[1], O_RDWR | O_CREAT | O_APPEND, 0666);

    write(fd, &program_time, 8);                                //write in log-file
    char bufy = ' ';
    write(fd, &bufy, 1);

    close(fd);
    close(fifo_descriptor);

    //printf("Got %ld bytes", bytesRead);

    return 0;
}
