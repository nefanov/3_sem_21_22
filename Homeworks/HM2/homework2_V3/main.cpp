#include "duplexPipe.h"
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char *argv[])
{
    dPipe dp;
    init_dPipe(&dp);

    pid_t pid = fork();

    if(pid)                 //parent
    {
        parent_Pipe_intit(&dp);

        time_t time1 = clock();

        int status = dp.operations.sendData(&dp, "input.txt");
        //int status = dp.operations.sendData(&dp, argv[1]);

        status = dp.operations.receiveData(&dp);
        parent_Pipe_terminate(&dp);

        time_t time2 = clock();

        double t = (double)(time2 - time1) / CLOCKS_PER_SEC;

        printf("\nProgram's time = %f secs\n", t);

        wait(NULL);
    }
    else {                  //child
        child_Pipe_intit(&dp);

        int status = dp.operations.receiveData(&dp);

        status = dp.operations.sendData(&dp, CHILD_TEMP_FILE_NAME);

        child_Pipe_terminate(&dp);

        exit(2);
    }

    return 0;
}

