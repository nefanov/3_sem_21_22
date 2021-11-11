//fifo

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

//msgq


#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

#define MSG_SIZE 1000010

typedef struct msgbuf
{
	long mtype;
	char mtext[MSG_SIZE];
} message_buf;
int main()
{
        int n;
        printf("Choose the size:");
        scanf("%d", &n);

	const pid_t pid = fork();
        if (pid < 0)
        {
                printf("fork failed!\n");
                return -1;
        }

        if (pid)
        { //parent
		int msqid;
        	int msgflg = IPC_CREAT | 0666;
        	key_t key;
       		message_buf sbuf;
        	size_t buf_length;

        	key = 1024;

        	msqid = msgget(key, msgflg);
        	sbuf.mtype = 1;
		int time1 = clock();
		if (n < 8192)
		{
			int size = n;
			for (int i = 0; i < size; i++)
			{
				sbuf.mtext[i] = 'a';
			}
			sbuf.mtext[size] = '\0';
			buf_length = size;

			msgsnd(msqid, &sbuf, buf_length, NULL);
		}
		else
		{
			int size = 8191;
			for (int i = 0; i < n / size; i++)
			{
				for (int j = 0; j < size; j++)
				{
					sbuf.mtext[i] = 'a';
				}
				sbuf.mtext[size] = '\0';
				buf_length = size;
				msgsnd(msqid, &sbuf, buf_length, NULL);
			}
			for (int i = 0; i < n % size; i++)
			{
				sbuf.mtext[i] = 'a';
			}
			sbuf.mtext[n % size] = '\0';
			buf_length = n % size;
			msgsnd(msqid, &sbuf, buf_length, NULL);
		}
        	int status;
        	waitpid(pid, &status, 0);
        	int time2 = clock();
        	printf("End time: %d %ld\n", time2-time1, CLOCKS_PER_SEC);
        }
        else
        { //child
		int msqid;
		key_t key;
		message_buf rbuf;
		key = 1024;
		msqid = msgget(key, 0666);
		if (n < 8192)
		{
			msgrcv(msqid, &rbuf, MSG_SIZE, 1, 0);
		}
		else
		{
			int size = 8191;
			for (int i = 0; i < n / size; i++)
			{
				msgrcv(msqid, &rbuf, size, 1, 0);
			}
			msgrcv(msqid, &rbuf, n % size, 1 ,0);
		}
        }

        return 0;
}

//smem


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
