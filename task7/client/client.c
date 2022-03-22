#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define PATH 256
#define CN_FIFO_REL "../../server/channels/"
#define CMD_SZ 1024
#define PATH_DIR "../server/"

char* tx_fifo, rx_fifo;

int create_fifo(const char* fifo_name, const char* perms) {
	char* path = calloc(PATH, sizeof(char));
	sprintf(path, "%s%s", PATH_DIR, fifo_name);
	int fd = open(path, perms);
	free(path);
	return fd;
}

int registration() {
	int fromclient_tx_fifo_fd = create_fifo("reg_serv", O_RDWR);
	int fromclient_rx_fifo_fd = create_fifo("response", O_RDWR);
	
	char client_msg[CMD_SZ] = "\0";
	sprintf(client_msg, "REGISTER %s %s\n", tx_fifo, rx_fifo);
	write(fromclient_tx_fifo_fd, client_msg, strlen(client_msg));

	char response[CMD_SZ] = "\0";
	read(fromclient_rx_fifo_fd, response, CMD_SZ);

	close(fromclient_tx_fifo_fd);
	close(fromclient_rx_fifo_fd);
	
	return 0;
}

char* make_path(int pid, char* name) {
	char* path = calloc(PATH, sizeof(char));
	sprintf(path, "%s%s_%d", PATH, name, pid);
	return path;
}

int main() {
	tx_fifo = make_path(getpid(), "tx_fifo");
	rx_fifo = make_path(getpid(), "rx_fifo");

	registration();

	free(tx_fifo);
	free(rx_fifo);
}
