#include <stdio.h>
#include <time.h>
#include <duplexPipe.h>

void echoTest(char* const filename)
{
    long long _file_size = 0;                     //узнаю размер файла
    struct stat _fileStatbuff;
    int fd = open(filename, O_RDWR);
    if(fd == -1){
        _file_size = -1;
    }
    else{
        if ((fstat(fd, &_fileStatbuff) != 0) || (!S_ISREG(_fileStatbuff.st_mode))) {
            _file_size = -1;
        }
        else{
            _file_size = _fileStatbuff.st_size;
        }
        close(fd);
    }

    if(_file_size < 0)
    {
        printf("Everything is bad");
        return;
    }

    printf("The test is being done with file %s, his size is %lld \nProgramm's output:\n\n", filename, _file_size);

    clock_t start = clock();

    dPipe testPipe;
    initPipe(&testPipe, filename);

    pid_t pid = fork();

    if(pid)                 //parent
    {
        long long bytes = testPipe.actions.sendData(&testPipe, 1);

        printf("Parent has send %lld/%lld bytes\n", bytes, _file_size);

        bytes = testPipe.actions.receiveData(&testPipe);

        printf("Parent has received %lld/%lld bytes\n", bytes, _file_size);

        wait(NULL);

        destructorPipe(&testPipe);
    }
    else {                  //child
        long long bytesC = testPipe.actions.receiveData(&testPipe);

        printf("Child has received %lld/%lld bytes\n", bytesC, _file_size);

        bytesC = testPipe.actions.sendData(&testPipe, 0);

        printf("Child now has send %lld/%lld bytes\n", bytesC, _file_size);

        destructorPipe(&testPipe);

        exit(2);
    }

    clock_t end = clock();

    printf("\n\nProgramm has been working for %lf secs\n", (double)((double)((end - start)) / CLOCKS_PER_SEC));
}

int main(int argn, char** argv)
{
    echoTest(argv[1]);

    return 0;
}
