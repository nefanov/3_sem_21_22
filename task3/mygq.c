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

