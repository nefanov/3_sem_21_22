#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char **parse_arg(char *str, int flag) {
	char delim0[] = " ,\n";
	char delim1[] = "|\n";
	int i = 0;
	char *saveptr;
	char **result = malloc(sizeof(char*) * 40);
	if (flag == 1) {
		for (char *token  = strtok_r(str, delim1, &saveptr); token != NULL; token = strtok_r(NULL, delim1, &saveptr)) {
			result[i] = token;
			i++;
			if (i % 40 == 0) {
				result = realloc(result, sizeof(char*) * 40);
			}
		}
	}
	else {
		for (char *cmd = strtok_r(str, delim0, &saveptr); cmd != NULL; cmd = strtok_r(NULL, delim0, &saveptr)) {
                	result[i] = cmd;
                	i++;
                	if (i % 40) {
                        	result = realloc(result, sizeof(char) * 40);
                	}
        	}
	}
	return result;
}

static void run_cmd(char **cmd_seq) {
	pid_t pid;
	int exit_status;
	int status;
	int fd[2];
	int i = 0;
	int fd_in;
	while (cmd_seq[i] != NULL) {
		pipe(fd);
		if ((pid = fork()) < 0) {
			printf("fork failed\n");
			exit(1);
		}
		else if (pid) {
			waitpid(pid, &status, 0);
			close(fd[1]);
			fd_in = fd[0];
			exit_status = WEXITSTATUS(status);
			i++;
		}
		else {
			char **args = parse_arg(cmd_seq[i], 0);
			dup2(fd_in, 0);
			if (cmd_seq[i+1] != NULL) {
				dup2(fd[1], 1);
			}
			close(fd[0]);
			execvp(args[0], args);
			printf("EXECUTION failed\n");
			exit(2);
		}
	}
	return;
}

int main () {
	while (1) {
		char *cmd;
		size_t trash;
		getline(&cmd, &trash, stdin);
		char **cmd_seq = parse_arg(cmd, 1);
		run_cmd(cmd_seq);
	}
}
