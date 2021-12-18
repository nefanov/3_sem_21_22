#include "mq_main.h"

int get_msg_id(const char *path) {
  key_t key = ftok(path, 1);
  if (key == -1) {
    perror("ftok error");
    exit(1);
  }

  int msg_id = msg_get(key, IPC_CREAT | 0666);
  if(msg_id == -1) {
    perror("msg_get error");
    exit(1);
  }

  return msg_id;
}

void send(int msg_id, char *argv[]) {
  // Get file descriptor
  int fin = open(argv[1], O_RDONLY);
  if (fin == -1) {
    perror("open fd error");
    exit(1);
  }

  // Initialization
  Message s_msg;
  s_msg.mtype = STD_MSG_TYPE;
  char *buf = malloc(MSG_TEXT_SZ);

  // Msg creation
  size_t read_sz = MSG_TEXT_SZ;
  while (1) {
    // Read data from file and write in msg
    read_sz = read(fin, buf, MSG_TEXT_SZ);
    strcpy(s_msg.mtext, buf);

    if (read_sz != MSG_TEXT_SZ) {
      if (read_sz != 0) {
        // Send last msg
        msgsnd(msg_id, &s_msg, read_sz, 0);
//        printf("Sender: msg \"%s\" was sent.\n", s_msg.mtext);
      }

      // End of sending
      s_msg.mtype = LAST_MSG_TYPE;
      msgsnd(msg_id, &s_msg, 0, 0);
      printf("Sender: last msg was sent.\n");

      printf("Sender: EOF!\n");
      break;
    } else {
      // Msg sending
      msgsnd(msg_id, &s_msg, MSG_TEXT_SZ, 0);
//      printf("Sender: msg \"%s\" was sent.\n", s_msg.mtext);
    }
  }

  free(buf);
  close(fin);
}
void receive(int msg_id, char *argv[]) {
  // Get file descriptor
  remove(argv[2]);
  int fout = open(argv[2], O_WRONLY | O_CREAT);
  if (fout == -1) {
    perror("open fd error");
    exit(1);
  }

  // Initialization
  Message r_msg;
  size_t written_sz;
  int len;

  // Msg receiving
  while (1) {
    // Receive msg from queue and write in file
    len = (int)msgrcv(msg_id, &r_msg, MSG_TEXT_SZ,0,0);
    if (len < 0) {
      printf("Receiver: len < 0\n");
      exit(1);
    }

    // Check on last msg
    if (r_msg.mtype == LAST_MSG_TYPE) {
      printf("Receiver: last msg was received.\n");

      printf("Receiver: deleting msg queue..\n");
      msgctl(msg_id, IPC_RMID, (struct msqid_ds *) NULL);

      break;
    } else {
//      printf("Receiver: msg \"%s\" was received.\n", r_msg.mtext);

      written_sz = write(fout, r_msg.mtext, len);
      if (written_sz == -1) {
        printf("Bad write of file\n");
        exit(1);
      }
    }
  }

  close(fout);
}

int main(int argc, char *argv[]) {
  int msg_id = get_msg_id(argv[0]);

  pid_t pid = fork();
  if(pid < 0) {
    perror("fork error");
    exit(1);
  }

  if (pid == 0) {
    send(msg_id, argv);
  } else {
    receive(msg_id, argv);
  }

  return 0;
}