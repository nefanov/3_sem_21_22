#include <duplexPipe.h>

size_t receiveData(dPipe* self)
{
    int sourcePipe;
    int fdTemp;
    char const* Name;

    if(self->parOrChild)        //parent
    {
        sourcePipe = self->fdBack[0];
        fdTemp = open(PARENT_TEMP_FILE_NAME, O_WRONLY);

        if(fdTemp < 0)
        {
            return -1;
        }

        Name = "Parent";
    }
    else {                      //child
        sourcePipe = self->fdDirect[0];
        fdTemp = open(CHILD_TEMP_FILE_NAME, O_WRONLY);

        if(fdTemp < 0)
        {
            return -1;
        }

        Name = "Child";
    }

    long const bufCapacity = BUF_CAPACITY;
    char buf[bufCapacity];
    int bytesCurRead = 0;
    long long bytesRead = 0;

    bytesCurRead = read(sourcePipe, buf, bufCapacity);
    bytesRead += bytesCurRead;

    if(buf[bytesCurRead-1] == '~')
        write(fdTemp, buf, bytesCurRead - 1);
    else
        write(fdTemp, buf, bytesCurRead);

    while (buf[bytesCurRead-1] != '~') {                           //signal that input has ended is \0
        bytesCurRead = read(sourcePipe, buf, bufCapacity);
        bytesRead += bytesCurRead;


        if(buf[bytesCurRead-1] == '~')
            write(fdTemp, buf, bytesCurRead - 1);
        else
            write(fdTemp, buf, bytesCurRead);
    }

    printf("%s have read %ld bytes\n", Name, bytesRead - 1);          //count without \0
    close(fdTemp);
    return bytesRead;
}

size_t sendData(dPipe* self, char* sourceFile)
{
    int sourcePipe;
    char const* Name;
    int sourceFileFd = open(sourceFile, O_RDONLY);

    if(sourceFileFd < 0)
    {
        return -1;
    }

    if(self->parOrChild)        //parent
    {
        sourcePipe = self->fdDirect[1];

        Name = "Parent";
    }
    else {                      //child
        sourcePipe = self->fdBack[1];

        Name = "Child";
    }

    long const bufCapacity = BUF_CAPACITY;
    char buf[bufCapacity];
    int bytesCurWritten = 0;
    int bytesCurRead = 1;
    long long bytesWritten = 0;

    while (bytesCurRead > 0) {
        bytesCurRead = read(sourceFileFd, buf, bufCapacity);

        if(bytesCurRead)
            bytesCurWritten = write(sourcePipe, buf, bytesCurRead);

        bytesWritten += bytesCurWritten;
        bytesCurWritten = 0;
    }

    char t = '~';
    write(sourcePipe, &t, 1);                                      //signal of end

    close(sourceFileFd);
    printf("%s has sent %ld bytes\n", Name, bytesWritten);
    return bytesWritten;
}

int init_dPipe(dPipe* p)
{
    int fd1[2]; int fd2[2];
    int statusPipe = pipe(fd1);

    if(statusPipe < 0)
    {
        return -1;
    }

    statusPipe = pipe(fd2);

    if(statusPipe < 0)
    {
        return -1;
    }

    p->fdDirect[0] = fd1[0];
    p->fdDirect[1] = fd1[1];

    p->fdBack[0] = fd2[0];
    p->fdBack[1] = fd2[1];

    p->operations.receiveData = receiveData;
    p->operations.sendData = sendData;

    return 0;
}

int child_Pipe_intit(dPipe* p)
{
    p->parOrChild = 0;

    close(p->fdDirect[1]);
    close(p->fdBack[0]);

    int fdTemp = open(CHILD_TEMP_FILE_NAME, O_RDWR | O_CREAT | O_TRUNC, 0666);

    if(fdTemp < 0)
        return -1;

    close(fdTemp);

    return 0;
}

int parent_Pipe_intit(dPipe* p)
{
    p->parOrChild = 1;

    close(p->fdDirect[0]);          //parent writes in direct, read from back
    close(p->fdBack[1]);

    int fdTemp = open(PARENT_TEMP_FILE_NAME, O_RDWR | O_CREAT | O_TRUNC, 0666);

    if(fdTemp < 0)
        return -1;

    close(fdTemp);

    return 0;
}

void child_Pipe_terminate(dPipe* p)
{
    close(p->fdDirect[0]);
    close(p->fdBack[1]);
}

void parent_Pipe_terminate(dPipe* p)
{
    close(p->fdDirect[1]);
    close(p->fdBack[0]);
}

