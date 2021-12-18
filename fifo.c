#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <sys/wait.h>

#define FIFO "fifo_example"
int main()
{
	char buf[1000100];
  	int size;
	int n;
	printf("Choose the size:");
	scanf("%d", &n);
	printf("\n");
  	const pid_t pid = fork();
  	if (pid < 0)
 	{
    		printf("fork failed!\n");
    		return -1;
  	}

  	if (pid)
  	{ //parent
		int time1 = clock();
		if (n <= 1000000)
		{
		        mknod(FIFO, S_IFIFO |0666, 0);
                	int fd = open(FIFO, O_WRONLY);
			size = n;
			for (int i = 0; i < size; i++)
			{
				buf[i] = 'a';
			}
			buf[size] = '\0';
			write(fd, buf, size);
		}
		else
		{
                	mknod(FIFO, S_IFIFO | 0666, 0);
                	int fd = open(FIFO, O_WRONLY);
			size = 1000000;
			for (int i = 0; i < n / size; i++)
			{
				for (int j = 0; j < size; j++)
				{
					buf[j] = 'a';
				}
				buf[size] = '\0';
				write(fd, buf, size);
			}
			size = n % 1000000;
			if (size > 0)
			{
				for (int j = 0; j < size; j++)
				{
					buf[j] = 'a';
				}
				buf[size] = '\0';
				write(fd, buf, size);
			}
		}
		int status;
		waitpid(pid, &status, 0);
		int time2 = clock();
		printf("End time: %d %ld\n", time2-time1, CLOCKS_PER_SEC);
  	}
  	else
	{ //child
		if (n <= 1000000)
		{
			mknod(FIFO, S_IFIFO | 0666, 0);
                	int fd = open(FIFO, O_RDONLY);
			size = read(fd, buf, sizeof(buf) - 1);
			buf[size] = '\0';
		}
		else
		{
               		mknod(FIFO, S_IFIFO | 0666, 0);
                	int fd = open(FIFO, O_RDONLY);
			int step = 1000000;
			for (int i = 0; i < n / step; i++)
			{
				size = read(fd, buf, sizeof(buf) - 1);
				buf[size] = '\0';
			}
			if (n % step > 0)
			{
				size = read(fd, buf, sizeof(buf) - 1);
				buf[size] = '\0';
			}
		}
  	}
  	return 0;
}

