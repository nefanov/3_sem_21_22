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

int main(int argc, char* argv[]) {
	char fifo_name[100];
	struct message msg;
	msg.client_pid = getpid();
	MakeFIFOName(msg.client_pid, fifo_name, sizeof(fifo_name));
	mkfifo(fifo_name, 0666);
	int fd_s;
	fd_s = open(SERVER_NAME, O_WRONLY);
	strcpy(msg.data, argv[1]);
	write(fd_s, &msg, sizeof(msg));
	int fd_c, fd_c_trash;
	fd_c = open(fifo_name, O_RDONLY);
	fd_c_trash = open(fifo_name, O_WRONLY);
	read(fd_c, &msg, sizeof(msg));
	int fout = fileno(fopen(argv[2], "w"));
	size_t len;

	while (1) {
		read(fd_c, msg.data, MSG_SIZE);
		len = (size_t)(msg.data[0]);
		if (len != MSG_SIZE) {
			if (len != 0) {
				write(fout, msg.data + PROT_SZ, len);
			}
			break;
		} else {
			write(fout, msg.data + PROT_SZ, len);
		}
	}
	close(fout);

	close(fd_s);
	close(fd_c_trash);
	close(fd_c);
	unlink(fifo_name);

	return 0;
}