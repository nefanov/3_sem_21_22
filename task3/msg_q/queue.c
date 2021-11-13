#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <string.h>

#define MSG_SIZE 128
#define STD 1
#define LAST_TYPE 255

typedef struct msg {
  long mtype;
  char mtext[MSG_SIZE];
} message;

void send(int msgid, char* argv[]) {
	int fin = open(argv[1], O_RDONLY);
	
	message msg;
	msg.mtype = STD;
	char* buf = malloc(MSG_SIZE);

	size_t r_size = MSG_SIZE;
	while (1) {
		r_size = read(fin, buf, MSG_SIZE);
		strcpy(msg.mtext, buf);

		if (r_size != MSG_SIZE) {
			if (r_size != 0) {
				msgsnd(msgid, &msg, r_size, 0);
			}

			msg.mtype = LAST_TYPE;
			msgsnd(msgid, &msg, 0, 0);
			break;
		} else {
			msgsnd(msgid, &msg, MSG_SIZE, 0);
		}
	}

	free(buf);
	close(fin);
}

void receive(int msgid, char* argv[]) {
	remove(argv[2]);
	int fout = open(argv[2], O_WRONLY | O_CREAT);

	message r_msg;
	size_t w_size;
	int len;

	while (1) {
		len = (int)msgrcv(msgid, &r_msg, MSG_SIZE, 0, 0);
		if (len < 0) {
			exit(1);
		}

		if (r_msg.mtype == LAST_TYPE) {
			msgctl(msgid, IPC_RMID, (struct msqid_ds *) NULL);
			break;
		} else {
			w_size = write(fout, r_msg.mtext, len);
		}
	}

	close(fout);
}

int getmsgid(const char* path) {
	key_t key = ftok(path, 1);
	
	int msgid = msgget(key, IPC_CREAT | 0666);

	return msgid;
}

int main(int argc, char* argv[]) {
	key_t key = ftok(argv[0], 1);
	int msgid = msgget(key, IPC_CREAT | 0666);

	pid_t pid = fork();
	if (pid == 0) {
		send(msgid, argv);
	} else {
		receive(msgid, argv);
	}
	return 0;
}
