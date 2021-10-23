#include <duplexPipe.h>

size_t sendDataChild(dPipe* this, _Bool flag)
{
    long long bytesSent = 0;
    char buf[INTERMEDIATE_BUF_SIZE];
    int fd;

    if(flag)
        fd = open(this->filename, O_RDONLY);        //открываем наш файл, откуда читаем
    else
    {
        fd = open("InterChild.txt", O_RDWR | O_CREAT | O_APPEND, 0666);                     //читаем с промеж. файла
    }

    if(fd == -1)
    {
        printf("Can't open file\n");
        return 0;
    }

    long curBytesWritten = read(fd, buf, INTERMEDIATE_BUF_SIZE);

    while (curBytesWritten > 0) {

        if(curBytesWritten <= 0)
        {
            printf("Unable to read from file, %lld bytes have been sent by child\n", bytesSent);

            return bytesSent;
        }

        long checksum = write(this->fdBack[1], buf, curBytesWritten);

        if(checksum <= 0)
        {
            printf("Unable to write at pipe, %lld bytes have been sent by child\n", bytesSent);

            return bytesSent;
        }

        bytesSent += checksum;

        if(curBytesWritten != INTERMEDIATE_BUF_SIZE)
        {
            //buf[0] = '\0';
            //write(this->fdBack[1], buf, 1);
            break;
        }

        curBytesWritten = read(fd, buf, INTERMEDIATE_BUF_SIZE);
    }

    close(fd);

    return bytesSent;
}

size_t receiveDataChild(dPipe* this)
{
    unsigned long long dataSize = 0;
    char buf[INTERMEDIATE_BUF_SIZE];

    long numOfBytes = 1;    //выводим данные из pipe

    int intermedFile = open("InterChild.txt", O_RDWR | O_CREAT | O_APPEND, 0666);

    numOfBytes = read(this->fdDirect[0], buf, INTERMEDIATE_BUF_SIZE);
    while (numOfBytes > 0) {
        write(intermedFile, buf, numOfBytes);

        write(1, buf, numOfBytes);

        dataSize += numOfBytes;

        if(numOfBytes != INTERMEDIATE_BUF_SIZE)
        {
            buf[0] = '\n';
            write(1, buf, 1);
            break;
        }

        numOfBytes = read(this->fdDirect[0], buf, INTERMEDIATE_BUF_SIZE);
    }

    close(intermedFile);
    return dataSize;
}

size_t receiveDataParent(dPipe* this)
{
    pid_t envPid = getpid();

    if(!this->coordinated)
    {
        if(this->parentPid != envPid)               //это ребёнок, настраиваем его
        {
            this->parentPid = -1;                   //из ребёнка не должно быть доступа к pid родителя

            close(this->fdDirect[1]);
            close(this->fdBack[0]);

            this->actions.receiveData = receiveDataChild;
            this->actions.sendData = sendDataChild;

            this->coordinated = 1;

            return receiveDataChild(this);
        }
        else
        {
            close(this->fdDirect[0]);
            close(this->fdBack[1]);

            this->coordinated = 1;
        }
    }

    unsigned long long dataSize = 0;
    char buf[INTERMEDIATE_BUF_SIZE];

    int intermedFile = open("InterParent.txt", O_RDWR | O_CREAT | O_APPEND, 0666);

    long numOfBytes = read(this->fdBack[0], buf, INTERMEDIATE_BUF_SIZE);    //выводим данные из pipe
    while (numOfBytes > 0) {
        write(intermedFile, buf, numOfBytes);
        write(1, buf, numOfBytes);

        dataSize += numOfBytes;

        if(numOfBytes != INTERMEDIATE_BUF_SIZE)
        {
            buf[0] = '\n';
            write(1, buf, 1);
            break;
        }

        numOfBytes = read(this->fdBack[0], buf, INTERMEDIATE_BUF_SIZE);
    }

    close(intermedFile);
    return dataSize;
}

size_t sendDataParent(dPipe* this, _Bool flag)
{
    pid_t envPid = getpid();

    if(!this->coordinated)
    {
        if(this->parentPid != envPid)               //это ребёнок, настраиваем его
        {
            this->parentPid = -1;                   //из ребёнка не должно быть доступа к pid родителя

            close(this->fdDirect[1]);
            close(this->fdBack[0]);

            this->actions.receiveData = receiveDataChild;
            this->actions.sendData = sendDataChild;

            this->coordinated = 1;

            return sendDataChild(this, flag);
        }
        else
        {
            close(this->fdDirect[0]);
            close(this->fdBack[1]);

            this->coordinated = 1;
        }
    }


    long long bytesSent = 0;
    char buf[INTERMEDIATE_BUF_SIZE];
    int fd;

    if(flag)
        fd = open(this->filename, O_RDONLY);        //открываем наш файл, откуда читаем
    else
    {
        fd = open("InterParent.txt", O_RDWR | O_CREAT | O_APPEND, 0666);                       //читаем с промеж. файла
    }

    if(fd == -1)
    {
        printf("Can't open file\n");
        return 0;
    }

    long curBytesWritten = read(fd, buf, INTERMEDIATE_BUF_SIZE);

    while (curBytesWritten > 0) {

        if(curBytesWritten <= 0)
        {
            printf("Unable to read from file, %lld bytes have been sent by parent\n", bytesSent);

            return bytesSent;
        }

        long checksum = write(this->fdDirect[1], buf, curBytesWritten);

        if(checksum <= 0)
        {
            printf("Unable to write at pipe, %lld bytes have been sent by parent\n", bytesSent);

            return bytesSent;
        }

        bytesSent += checksum;

        if(curBytesWritten != INTERMEDIATE_BUF_SIZE)
        {
            //buf[0] = '\0';
            //write(this->fdDirect[1], buf, 1);
            break;
        }

        curBytesWritten = read(fd, buf, INTERMEDIATE_BUF_SIZE);
    }

    close(fd);

    return bytesSent;
}

_Bool initPipe(dPipe* this, char* const fileName)
{
    this->parentPid = getpid();
    this->coordinated = 0;

    int fd1[2]; int fd2[2];

    if(pipe(fd1) < 0)
    {
        printf("Pipe creation's failed!\n");
        this->parentPid = -1;

        return -1;
    }
    if(pipe(fd2) < 0)
    {
        printf("Pipe creation's failed!\n");
        this->parentPid = -1;

        return -1;
    }

    this->fdDirect[0] = fd1[0];
    this->fdDirect[1] = fd1[1];
    this->fdBack[0] = fd2[0];
    this->fdBack[1] = fd2[1];

    this->filename = (char*)calloc(strlen(fileName), sizeof (char));
    strcpy(this->filename, fileName);

    this->actions.receiveData = receiveDataParent;
    this->actions.sendData = sendDataParent;
    this->actions.destructor = destructorPipe;

    //FILE* f = fopen("InterChild.txt", "w");
    //FILE* s = fopen("InterParent.txt", "w");

    //fclose(f);
    //fclose(s);

    return 1;
}

void destructorPipe(dPipe* this)
{
    if(this->filename != NULL)
        free(this->filename);

    if(this->parentPid != -1)
    {
        close(this->fdDirect[1]);
        close(this->fdBack[0]);
    }
    else {
        close(this->fdDirect[0]);
        close(this->fdBack[1]);
    }

    if(this->parentPid != -1)
    {
        FILE* f = fopen("InterChild.txt", "w");
        FILE* s = fopen("InterParent.txt", "w");

        fclose(f);
        fclose(s);
    }

    this->coordinated = 0;
    this->parentPid = -1;
}
