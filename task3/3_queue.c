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

typedef struct msgbuf {
	long mtype;
	char mtext[MSG_SIZE];
} message;

int getmsgid(const char *path) {
  key_t key = ftok(path, 1);
  int msgid = msgget(key, IPC_CREAT | 0666);
  return msgid;
}

void send(int msgid, char *argv[]) {
  int fin = open(argv[1], O_RDONLY);
  message s_msg;
  s_msg.mtype = STD;
  char *buf = malloc(MSG_SIZE);
  size_t read_sz = MSG_SIZE;
  while (1) {
    read_sz = read(fin, buf, MSG_SIZE);
    strcpy(s_msg.mtext, buf);
    if (read_sz != MSG_SIZE) {
      if (read_sz != 0) {
        msgsnd(msgid, &s_msg, read_sz, 0);
      }
      s_msg.mtype = LAST_TYPE;
      msgsnd(msgid, &s_msg, 0, 0);
      printf("Sender: sent.\n");
      printf("Sender: EOF!\n");
      break;
    } else {
      msgsnd(msgid, &s_msg, MSG_SIZE, 0);
    }
  }

  free(buf);
  close(fin);
}
void receive(int msgid, char *argv[]) {
  remove(argv[2]);
  int fout = open(argv[2], O_WRONLY | O_CREAT);
  message r_msg;
  size_t written_sz;
  int len;
  while (1) {
    len = (int)msgrcv(msgid, &r_msg, MSG_SIZE,0,0);
    if (r_msg.mtype == LAST_TYPE) {
      printf("Receiver: received.\n");
      msgctl(msgid, IPC_RMID, (struct msqid_ds *) NULL);
      break;
    } else {
      written_sz = write(fout, r_msg.mtext, len);
    }
  }
  close(fout);
}

int main(int argc, char *argv[]) {
  int msgid = getmsgid(argv[0]);
  pid_t pid = fork();
  if (pid == 0) {
    send(msgid, argv);
  } else {
    receive(msgid, argv);
  }

  return 0;
}
