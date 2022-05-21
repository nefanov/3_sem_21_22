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

int make_connection(int fd_server, struct message* msg) {
	int fd_client;
	
	read(fd_server, msg, sizeof(*msg));
	MakeFIFOName(msg->client_pid, fifo_name, sizeof(fifo_name));

	struct message connect;
	fd_client = open(fifo_name, O_WRONLY);
	strcpy(connect.data, "ready");
	write(fd_client, &connect, sizeof(connect)); 
	
	return fd_client;
}

int main(int argc, char* argv[]) {
	mkfifo(SERVER_NAME, 0666);
	
	int fd_server, fd_server_trash;
	fd_server = open(SERVER_NAME, O_RDONLY);
	fd_server_trash = open(SERVER_NAME, O_WRONLY);

	struct message msg;
	int fd_client = make_connection(fd_server, &msg);

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
        		write(fd_client, msg.data, read_sz + PROT_SZ);
      		}
      		printf("Server: EOF!\n");
      		break;
    	} else {
      		msg.data[0] = (char)read_sz;
      		write(fd_client, msg.data, read_sz + PROT_SZ);
    	}
  	}

	close(fin);
	close(fd_client);
	close(fd_server);

	return 0;
}
