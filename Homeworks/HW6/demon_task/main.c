#include <demon.h>

extern char* targetDirectory;
extern char* backupDirectory;
extern char* logfileName;
extern int T;
extern time_t lastDemonCheck;


int main(int argc, char* argv[])
{
    if(strcmp(argv[2], "-d") == 0)                          //demon mode
    {
        pid_t pid = fork();

        if(pid < 0)
        {
            return -1;
        }
        else if (pid == 0)
        {
            init(argv[1], 1);
            demonWork();
        }
        else {
            return 0;
        }
    }

    init(argv[1], 0);             //terminal mode
    demonWork();

    return 0;
}
