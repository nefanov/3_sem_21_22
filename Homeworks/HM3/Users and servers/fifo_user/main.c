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

    int fifo_descriptor = open(FIFO_NAME, O_WRONLY);

    if(fifo_descriptor < 0)
    {
        perror("Can't open fifo");
        return -1;
    }

    char buf[PIPE_BUF];                                 //temporary buf
    int sourceFile = open(argv[1], O_RDONLY);

    if(sourceFile < 0)
    {
        perror("Can't open source file");
        return -1;
    }

    long curBytesReadFromSource = 0;
    long curBytesWritten = 0;
    long bytesWritten = 0;

    curBytesReadFromSource = read(sourceFile, buf, PIPE_BUF);

    while (curBytesReadFromSource > 0) {
        curBytesWritten = write(fifo_descriptor, buf, curBytesReadFromSource);

        bytesWritten += curBytesWritten;
        curBytesReadFromSource -= curBytesWritten;

        while (curBytesReadFromSource > 0) {
            curBytesWritten = write(fifo_descriptor, buf + curBytesWritten, curBytesReadFromSource);
            bytesWritten += curBytesWritten;
            curBytesReadFromSource -= curBytesWritten;
        }

        curBytesReadFromSource = read(sourceFile, buf, PIPE_BUF);
    }

    close(fifo_descriptor);
    close(sourceFile);

    //printf("Write %ld bytes", bytesWritten);

    return 0;
}
