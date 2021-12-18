
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>

int main()
{
        int size;
        int n;
        printf("Choose the size:");
        scanf("%d", &n);
        printf("\n");

	size = n;
	void *addr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	char *shared = addr;
        const pid_t pid = fork();
        if (pid < 0)
        {
                printf("fork failed!\n");
                return -1;
        }

        if (pid)
        { //parent
		int time1 = clock();
                for (int i = 0; i < size; i++)
                {
                        shared[i] = 'a';
                }
                shared[size] = '\0';

                int status;
                waitpid(pid, &status, 0);
                int time2 = clock();
                printf("End time: %d %ld\n", time2-time1, CLOCKS_PER_SEC);
        }
        else
        { //child
        }

        return 0;
}
