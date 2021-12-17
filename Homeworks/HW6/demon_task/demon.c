#include<demon.h>

char* targetDirectory;
char* backupDirectory;
char* logfileName;
int T;                               //период проверки
time_t lastDemonCheck;

void sigHandler(int sigNum)            //for stopping demon
{
    FILE* log = fopen(logfileName, "a+");

    time_t curTime = time(NULL);

    fprintf(log, "\nDemon has been interupted. Time: %s", ctime(&curTime));

    fclose(log);

    free(targetDirectory);
    free(backupDirectory);
    free(logfileName);

    exit(1);
}

void tryLater()
{
    FILE* log = fopen(logfileName, "a");

    if(log != NULL)
    {
        fprintf(log, "Can't open dir\n");
        fclose(log);
    }
    sleep(T);
}

_Bool isTextFile(char* fileName)
{
    char tempFileName[20];
    strcpy(tempFileName, "isTextTemp.txt");

    int tempFd = open("isTextTemp.txt", O_RDWR | O_CREAT, 0666);

    char fileQuery[PATH_MAX*2];
    strcpy(fileQuery,"file -b ");
    strcpy(fileQuery + strlen(fileQuery), fileName);
    strcpy(fileQuery + strlen(fileQuery), " > ");
    strcpy(fileQuery + strlen(fileQuery), tempFileName);

    int status = system(fileQuery);
    if(status < 0)
        return -1;

    char answerBuf[1024];
    read(tempFd, answerBuf, 1024);

    if(strlen(answerBuf) == 0)
        return -1;

    char* nextWord = strtok(answerBuf, " ,.");

    while (nextWord != NULL) {
        if(strcmp(nextWord, "text") == 0)
        {
            close(tempFd);
            remove("isTextTemp.txt");
            return 1;
        }

        nextWord = strtok(NULL, " ,.");
    }

    close(tempFd);
    remove("isTextTemp.txt");

    return -1;
}

short copyFile(char* absoluteTargetPath, char* absoluteBackfileName, _Bool createOrRewrite)
{
    if(!createOrRewrite)
    {
        remove(absoluteBackfileName);
    }

    int backupFd = open(absoluteBackfileName, O_RDWR | O_CREAT, 0666);
    int originFd = open(absoluteTargetPath, O_RDONLY, 0666);

    if(originFd < 0)        //can't read this file
        return -1;

    char buf[10000];

    int bytesRead = read(originFd, buf, 10000);

    while (bytesRead > 0) {
        write(backupFd, buf, bytesRead);

        bytesRead = read(originFd, buf, 10000);
    }

    close(backupFd);
    close(originFd);

    return 0;
}

short diffTest(char* absoluteTargetPath, char* fileName)
{
   char absoluteBackfileName[PATH_MAX];
   strcpy(absoluteBackfileName, backupDirectory);
   strcpy(absoluteBackfileName+strlen(absoluteBackfileName), fileName);

   char tempNameErr[PATH_MAX];
   strcpy(tempNameErr, "tempErr.txt");
   char tempName[PATH_MAX];
   strcpy(tempName, "temp.txt");

   int tempFileFd = open("temp.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
   int tempErrFileFd = open("tempErr.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
   close(tempErrFileFd);
   close(tempFileFd);


   char diffQuery1[3*PATH_MAX];
   strcpy(diffQuery1, "diff ");
   strcpy(diffQuery1 + strlen(diffQuery1), absoluteTargetPath);
   strcpy(diffQuery1 + strlen(diffQuery1), " ");
   strcpy(diffQuery1 + strlen(diffQuery1), absoluteBackfileName);
   strcpy(diffQuery1 + strlen(diffQuery1), " 2> ");
   strcpy(diffQuery1 + strlen(diffQuery1), tempNameErr);

   int diffStatus = system(diffQuery1);

   char diffQuery2[3*PATH_MAX];
   strcpy(diffQuery2, "diff ");
   strcpy(diffQuery2 + strlen(diffQuery2), absoluteTargetPath);
   strcpy(diffQuery2 + strlen(diffQuery2), " ");
   strcpy(diffQuery2 + strlen(diffQuery2), absoluteBackfileName);
   strcpy(diffQuery2 + strlen(diffQuery2), " > ");
   strcpy(diffQuery2 + strlen(diffQuery2), tempName);

   diffStatus = system(diffQuery2);

   if(diffStatus < 0)
       return -1;

   tempFileFd = open("temp.txt", O_RDWR | O_APPEND, 0666);
   tempErrFileFd = open("tempErr.txt", O_RDWR | O_APPEND, 0666);


   char diffErrResult[10];
   memset(diffErrResult, 0 ,10);
   read(tempErrFileFd, diffErrResult, 10);
   char diffResult[10];
   memset(diffResult, 0 ,10);
   read(tempFileFd, diffResult, 10);

   int logFileFd = open(logfileName, O_WRONLY | O_APPEND);

   if(diffErrResult[0] == 'd')                 //no backup for this file
   {
       int copyStatus = copyFile(absoluteTargetPath, absoluteBackfileName, 1);
       if(copyStatus < 0)
           return -1;

       char log[1024];
       strcpy(log, fileName); strcpy(log + strlen(log), "\n Backuped\n");
       write(logFileFd, log, strlen(log));
   }
   else if(diffResult[0] >= '0' && diffResult[0] <= '9')            //new changes
   {
       copyFile(absoluteTargetPath, absoluteBackfileName, 0);

       char log[1024];
       strcpy(log, fileName); strcpy(log + strlen(log), "\n Changes saved\n");
       write(logFileFd, log, strlen(log));
   }
   else if(diffResult[0] == '\0')                                   //no changes
   {
       char log[1024];
       strcpy(log, fileName); strcpy(log + strlen(log), "\n No changes\n");
       write(logFileFd, log, strlen(log));
   }

   utime(absoluteBackfileName, NULL);                          //touch file to update his access time

   close(tempFileFd);
   close(tempErrFileFd);
   close(logFileFd);
   remove("temp.txt");
   remove("tempErr.txt");
   return 1;
}

void SearchDirectory(const char *name, char* dirBase, int benchmark) {
    DIR *dir = opendir(name);               //Assuming absolute pathname here.
    if(dir) {
        char Path[PATH_MAX]; char* EndPtr = Path;
        struct stat info;                //Helps us know about stuff
        struct dirent *e;
        strcpy(Path, name);                  //Copies the current path to the 'Path' variable.
        EndPtr += strlen(name);              //Moves the EndPtr to the ending position.
        while ((e = readdir(dir)) != NULL) {  //Iterates through the entire directory.
            strcpy(EndPtr, e -> d_name);       //Copies the current filename to the end of the path, overwriting it with each loop.

            if (!stat(Path, &info)) {         //stat returns zero on success.
                if (S_ISDIR(info.st_mode)) {  //Are we dealing with a directory?
                    if (!strcmp(e -> d_name,".." ) || !strcmp(e -> d_name,"." )) //Ignore if it is the same or level above directory
                        continue;

                    if(benchmark + 1 > 2)                                        // controlling dive level
                        continue;

                    char dirBaseCopy[PATH_MAX];
                    strcpy(dirBaseCopy, dirBase);

                    strcpy(dirBase + strlen(dirBase), e->d_name);
                    strcpy(dirBase + strlen(dirBase), "_");

                    SearchDirectory(strcat(Path,"/"), dirBase, benchmark + 1);   //Calls this function AGAIN, this time with the sub-name.
                    memset(dirBase, 0, strlen(dirBase));
                    strcpy(dirBase, dirBaseCopy);
                } else if (S_ISREG(info.st_mode)) { //Or did we find a regular file?
                    if(isTextFile(Path))
                    {
                        char fullFileName[PATH_MAX];
                        strcpy(fullFileName, dirBase);
                        strcpy(fullFileName + strlen(fullFileName), e->d_name);
                        diffTest(Path, fullFileName);
                    }
                }
            } else {

            }
        }
    }

    return;
}

short init(char* configFile, _Bool isDemon)
{
    FILE* conf = fopen(configFile, "r");
    char targetPid[10];
    char backupPath[PATH_MAX];
    double t;

    fscanf(conf, "%s %s %lf", targetPid, backupPath, &t);
    backupDirectory = (char*)calloc(strlen(backupPath), sizeof (char));
    strcpy(backupDirectory, backupPath);

    T = t;

    fclose(conf);

    int tempFileFd = open("tempFile", O_RDWR | O_CREAT, 0666);

    int stdoutCopy = dup(1);
    dup2(tempFileFd, 1);

    close(tempFileFd);

    /*char targetWD_command[2048];                          //здесь можно смотреть cwd процесса, но я изменил на конкретную директорию с файлами

    strcpy(targetWD_command, "readlink -e /proc/");
    strcat(targetWD_command, targetPid);
    strcat(targetPid, "/cwd");

    int status = system(targetWD_command);

    if(status < 0)
        return -1;

    targetDirectory = (char*)calloc(PATH_MAX, sizeof (char));
    tempFileFd = open("tempFile", O_RDWR);

    read(tempFileFd, targetDirectory, PATH_MAX);
    targetDirectory[strlen(targetDirectory)-1] = '/';*/

    targetDirectory = (char*)calloc(PATH_MAX, sizeof (char));
    strcpy(targetDirectory, "/home/lliric/Homeworks/HM6/");

    dup2(stdoutCopy, 1);
    close(tempFileFd);
    remove("tempFile");

    FILE* demonFile = fopen("demon_info.txt", "w");
    time_t curTime = time(NULL);

    pid_t dPid = getpid();

    fprintf(demonFile, "Demon's pid is %d \nTime of creation: %s", dPid, ctime(&curTime));
    fclose(demonFile);

    logfileName = (char*)calloc(PATH_MAX, sizeof (char));
    getcwd(logfileName, PATH_MAX);
    strcat(logfileName, "/demonLog.txt");

    remove(logfileName);

    int logFd = open("demonLog.txt", O_RDWR | O_CREAT, 0666);
    close(logFd);

    lastDemonCheck = time(NULL);

    if(isDemon)
    {
        umask(0);
        setsid();
        chdir("/");
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    //прицепить хэндлер на события
    void (*sigStatus)(int);
    sigStatus = signal(SIGINT, sigHandler);
    if(sigStatus == SIG_ERR)
        return -1;

    return 1;
}

void demonWork()
{
    while (1) {
        sleep(T);

        FILE* logF = fopen(logfileName, "a+");
        time_t curTime = time(NULL);
        fprintf(logF, "Next iteration on %s\n", ctime(&curTime));
        fclose(logF);

        char* s = (char*)calloc(PATH_MAX, sizeof (char));
        SearchDirectory(targetDirectory, s, 1);
        free(s);

        DIR *dir = opendir(backupDirectory);

        if(dir == NULL)
            continue;

        struct stat info;
        struct dirent *e;

        logF = fopen(logfileName, "a+");

        char nextFileName[PATH_MAX];
        char* endPtr = nextFileName;
        strcpy(nextFileName, backupDirectory);
        endPtr += strlen(nextFileName);

        while ((e = readdir(dir)) != NULL) {

            if (!strcmp(e -> d_name,".." ) || !strcmp(e -> d_name,"." ) || (strcmp(e->d_name, logfileName) == 0)) //Ignore if it is the same or level above directory
                continue;

            strcpy(endPtr, e->d_name);

            stat(nextFileName, &info);

            if(info.st_atim.tv_sec <= lastDemonCheck)         // remove files, that has been removed in origin directory
            {
                remove(nextFileName);

                fprintf(logF, "%s\n File removed\n", e->d_name);
            }
        }

        fprintf(logF, "----------------------------------\n");

        fclose(logF);
        closedir(dir);
        lastDemonCheck = time(NULL);

        //kill(getpid(), SIGINT);
    }
}
