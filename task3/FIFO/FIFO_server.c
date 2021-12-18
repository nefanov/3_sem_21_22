#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>

#define MSG_SIZE 128
#define PROT_SZ 1
#define CHANNELS_REL_PATH "../channels/"
#define SERVER_NAME "../channels/fifo_server"

struct message {
	pid_t client_pid;
	char data[MSG_SIZE];
};

int MakeFIFOName(pid_t pid, char *name, size_t name_max) {
  snprintf(name, name_max, "%sfifo%d", CHANNELS_REL_PATH, pid);
  return 0;
}

char fifo_name[100] = "\0";

int make_connection(int fd_s, struct message* msg) {
	int fd_c;

	read(fd_s, msg, sizeof(*msg));
	MakeFIFOName(msg->client_pid, fifo_name, sizeof(fifo_name));

	struct message connect;
	fd_c = open(fifo_name, O_WRONLY);
	strcpy(connect.data, "ready");
	write(fd_c, &connect, sizeof(connect)); 

	return fd_c;
}

int main(int argc, char* argv[]) {
	mkfifo(SERVER_NAME, 0666);

	int fd_s, fd_s_trash;
	fd_s = open(SERVER_NAME, O_RDONLY);
	fd_s_trash = open(SERVER_NAME, O_WRONLY);

	struct message msg;
	int fd_c = make_connection(fd_s, &msg);

	int fin = open(msg.data, O_RDONLY);
	if (fin == -1) {
		perror("open error");
		exit(1);
	}

	size_t read_sz;

  	while (1) {
    	read_sz = read(fin, msg.data+PROT_SZ, MSG_SIZE);
    	if (read_sz != MSG_SIZE) {
      		if (read_sz != 0) {
        		msg.data[0] = (char)read_sz;
        		write(fd_c, msg.data, read_sz + PROT_SZ);
      		}
      		printf("Server: EOF!\n");
      		break;
    	} else {
      		msg.data[0] = (char)read_sz;
      		write(fd_c, msg.data, read_sz + PROT_SZ);
    	}
  	}

	close(fin);
	close(fd_c);
	close(fd_s);

	return 0;
}