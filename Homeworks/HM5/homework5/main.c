#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <time.h>


#define INPUT_FILE_NAME "input.txt"
#define OUTPUT_FILE_NAME "output.txt"
#define SIGNAL_NUMBER (SIGRTMIN + 1)
#define SPECIAL_CHARACTER '~'

int outputFileFd;
_Bool endOfTransaction = 0;
int* shm_segment;

void handler(int signal, siginfo_t* sig_info, void* uncontext)
{
    int intData = sig_info->si_value.sival_int;
    char* data = &intData;
    int len = 0;
    _Bool endOfTrans = 0;

    for (int i = 0; i < 4; ++i) {
        if(*(data + i) == SPECIAL_CHARACTER)
        {
            endOfTrans = 1;
            break;
        }
        else ++len;
    }

    char t = '\n';
    write(outputFileFd, data, len);
    //write(1, data, len);
    //write(1, &t, 1);
    endOfTransaction = endOfTrans;

    if(!endOfTrans)
    {
        *shm_segment = 1;
    }
    else
    {
        *shm_segment = 1;
    }


}

int main(int argn, char* const argv[])
{
    outputFileFd = open(OUTPUT_FILE_NAME, O_WRONLY | O_CREAT, 0666);

    if(outputFileFd < 0)
    {
        return -1;
    }

    struct sigaction sigact_struct;

    sigact_struct.sa_sigaction = handler;

    sigset_t sigSet;
    sigemptyset(&sigSet);
    sigaddset(&sigSet, SIGNAL_NUMBER);
    sigact_struct.sa_mask = sigSet;

    sigact_struct.sa_flags = SA_SIGINFO;

    if(sigaction(SIGNAL_NUMBER, &sigact_struct, NULL) < 0)
        return -1;

    int shm_id = shmget(67, 4, 0666 | IPC_CREAT);                             //for sync
    shm_segment = (int*)(shmat(shm_id, NULL, 0));
    *shm_segment = 0;

    pid_t pid = fork();

    if(pid)                     //parent
    {
        _Bool lastSend = 0;
        close(outputFileFd);

        int sourceFileFd = open(INPUT_FILE_NAME, O_RDONLY);

        if(sourceFileFd < 0)
        {
            kill(pid, SIGKILL);
            wait(NULL);
            close(outputFileFd);

            return -1;
        }

        int bytesRead = 1;
        int data;
        char chData[4];

        time_t  time1 = clock();

        bytesRead = read(sourceFileFd, chData, 4);

        union sigval val;
        if(bytesRead < 4)
        {
            chData[bytesRead] = SPECIAL_CHARACTER;
            bytesRead++;
        }

        char* d = &data;

        for (int i = 0; i < bytesRead; ++i) {
            *(d+i) = chData[i];
        }

        val.sival_int = data;

        sigqueue(pid, SIGNAL_NUMBER, val);

        while (*shm_segment != 1);
        *shm_segment = 0;

        while (1) {
            bytesRead = read(sourceFileFd, chData, 4);

            if(bytesRead <= 0)
            {

                if(!lastSend)
                {
                    chData[0] = SPECIAL_CHARACTER;
                    bytesRead++;

                    d = &data;

                    for (int i = 0; i < bytesRead; ++i) {
                        *(d+i) = chData[i];
                    }

                    val.sival_int = data;

                    sigqueue(pid, SIGNAL_NUMBER, val);
                }

                break;
            }

            if(bytesRead < 4)
            {
                chData[bytesRead] = SPECIAL_CHARACTER;
                lastSend = 1;
                bytesRead++;
            }

            d = &data;

            for (int i = 0; i < bytesRead; ++i) {
                *(d+i) = chData[i];
            }

            val.sival_int = data;

            sigqueue(pid, SIGNAL_NUMBER, val);

            if(chData[bytesRead] != SPECIAL_CHARACTER)
            {
                while (*shm_segment != 1);
                *shm_segment = 0;
            }


        }

        wait(NULL);

        time_t  time2 = clock();
        double time3 = (double)(time2 - time1);
        time3 /= CLOCKS_PER_SEC;

        printf("Transfer time: %lf\n", time3);

        close(sourceFileFd);
        shmdt((void*)shm_segment);
    }
    else {                      //child
        shm_segment = (int*)shmat(shm_id, NULL, 0);

        while (!endOfTransaction);

        close(outputFileFd);
        shmdt((void*)shm_segment);
        exit(2);
    }


    short flag = 0;
    int sourceFileFd = open(INPUT_FILE_NAME, O_RDONLY, 0666);
    outputFileFd = open(OUTPUT_FILE_NAME, O_RDONLY, 0666);
    char bufReadSource[1000]; char bufWriteSource[1000];
    int bytesReadSource; int bytesWriteSource; bytesReadSource = bytesWriteSource = 1;

    while (bytesReadSource > 0 && bytesWriteSource > 0) {
        bytesReadSource = read(sourceFileFd, bufReadSource, 1000);
        bytesWriteSource = read(outputFileFd, bufWriteSource, 1000);

        if(bytesReadSource == 0 || bytesWriteSource == 0)
            break;

        if(bytesReadSource < 0 || bytesWriteSource < 0)
        {
            flag = -1;
            break;
        }

        if(bytesReadSource != bytesWriteSource)
        {
            flag = 1;
            break;
        }

        for (int i = 0; i < bytesReadSource; ++i) {
            if(bufReadSource[i] != bufWriteSource[i])
            {
                flag = 1;
                break;
            }
        }
    }

    if(bytesReadSource != bytesWriteSource || flag == 1)
    {
        printf("File transffered with sum errors\n");
    }
    else if(flag == 0)
    {
        printf("File transffered normally\n");
    }
    else {
        printf("Some error occured with 'read'\n");
    }

    close(sourceFileFd);
    close(outputFileFd);
    shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}
