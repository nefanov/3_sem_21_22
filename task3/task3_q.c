#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

//#define MSG_SIZE 2048
// нужна структурка данных, в которой хранится тип сообщения и буфер
typedef struct msgbuf {
 long    mtype;
 char    mtext[MSG_SIZE];
} message_buf;



typedef struct buf_length {
 long    mtype;
 int    mlen;
} buf_length;



typedef struct death {
 long    mtype;
 int    flag;
} death;


void snd(message_buf sbuf, int msqid, int c) {
  buf_length buf;
  buf.mtype = 2;
  buf.mlen = c; // данные сообщения по-прежнему считаем '\0'-terminated - строкой.
  if (msgsnd(msqid, &buf, sizeof(int), 0) < 0) {
   printf ("%d, %d\n", msqid, buf.mlen);
   perror("msgsnd");
   exit(1);
 }
 if (msgsnd(msqid, &sbuf, buf.mlen, 0) < 0) {
   printf ("%d, %s, %d\n", msqid, sbuf.mtext, buf.mlen);
   perror("msgsnd");
   exit(1);
 }
}

void snd_death(int flag, int msqid) {
  death dead;
  dead.mtype = 3;
  dead.flag = flag;
  if (msgsnd(msqid, &dead, sizeof(int), 0) < 0) {
   printf ("%d, %d\n", msqid, dead.flag);
   perror("msgsnd");
   exit(1);
 }
}




int rcv(message_buf *sbuf, int msqid) {
  buf_length buf;
  death dead;
  msgrcv(msqid, &dead, sizeof(int), 3, 0);
  if (dead.flag)
    return 0;
  msgrcv(msqid, &buf, sizeof(int), 2, 0);
  msgrcv(msqid, sbuf, buf.mlen, 1, 0);
  return buf.mlen;
}



int main(int argc, char *argv[])
{
  int msqid;
  int msgflg = IPC_CREAT | 0666;
  

  key_t key = ftok("/tmp", 'S');

  if ((msqid = msgget(key, msgflg )) < 0) {
    perror("msgget");
    exit(1);
  }


  for (int j = 0; j < atoi(argv[1]); ++j) {
    pid_t pid = fork();
    if (pid) {
      FILE *fp;
      message_buf sbuf;
      sbuf.mtype = 1;
      fp = fopen("file", "rb");
      int c;
      while ((c = fread(sbuf.mtext, sizeof(char), MSG_SIZE, fp))) {
        snd_death(0, msqid);       
        snd(sbuf, msqid, c);
      }
      snd_death(1, msqid);
      fclose(fp);
      waitpid(pid, NULL, 0);
    }

    if (pid == 0) {
      FILE *f;
      message_buf  rbuf;
      f = fopen("res", "wb");
      int len;
      while ((len = rcv(&rbuf, msqid))) {
       fwrite(rbuf.mtext, sizeof(char), len, f);
     }
     fclose(f);
     return 0;
   }

 }
 msgctl(msqid, IPC_RMID, NULL);
 return 0;
}