#include "programs_starter.h"

double* executePrograms()
{
    char* sourceFileNames[3] = {SMALL_FILE_NAME, MEDIUM_FILE_NAME, BIG_FILE_NAME};
    char* logFileNames[3] = {SMALL_LOGFILE_NAME, MEDIUM_LOGFILE_NAME, BIG_LOGFILE_NAME};
    char* programNameServer[3] = {"./fifo_server", "./msgq_server", "./SM_server"};
    char* programNameUser[3] = {"./fifo_user", "./msgq_user", "./SH_user"};
    double* answers = new double[9];

    for (int i = 0; i < 3; ++i) {

        for (int j = 0; j < 3; ++j) {
            pid_t pid1 = fork();
            pid_t pid2;

            if(pid1 < 0)
            {
                perror("Can't fork");
                return NULL;
            }

            if(pid1)
                pid2 = fork();

            if(pid2 && pid2 < 0)
            {
                perror("Can't fork");
                return NULL;
            }

            if(pid1 == 0)
            {
                if(j == 2)
                    sleep(0.2);

                int statusServer = execlp(programNameServer[j], programNameServer[j], logFileNames[i], (char*)NULL);

                if(statusServer < 0)
                {
                    exit(-1);
                }

                exit(2);
            }
            else if(pid2 == 0){
                if(j != 2)
                    sleep(0.2);

                int statusUser = execlp(programNameUser[j], programNameUser[j], sourceFileNames[i], (char*)NULL);

                if(statusUser < 0)
                {
                    exit(-1);
                }

                exit(2);
            }

            else {
                int status1, status2;
                waitpid(pid1, &status1, 0);
                waitpid(pid2, &status2, 0);

                if(status1 < 0  || status2 < 0)
                {
                    perror("Process failed");
                    return NULL;
                }
            }
        }
    }

    char buf[1];
    double nextTime;

    for (int i = 0; i < 3; ++i) {
        int fd = open(logFileNames[i], O_RDWR);

        if(fd < 0)
        {
            perror("Can't open file");
            return NULL;
        }

        for (int j = 0; j < 3; ++j) {
            int bytesRead = read(fd, &nextTime, 8);

            if(bytesRead < 8)
            {
                perror("Can't read file");
                return NULL;
            }

            answers[3*i + j] = nextTime;
            read(fd, buf, 1);
        }

        close(fd);
    }

    return answers;
}
