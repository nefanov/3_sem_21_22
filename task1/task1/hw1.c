#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ARG 0
#define CMD 1



char** parse(char* s_0, int flag) {
	
	char** r = malloc(sizeof(char*)*100);
	char *rest = NULL;
	char *token;
	char* s = strdup(s_0);
	int i = 0;
	if (flag == ARG) {
		const char delimA[] = " ,\n";
		for (token = strtok_r(s, delimA, &rest); token != NULL; token = strtok_r(NULL, delimA, &rest)) {   
			r[i++] = token;
			if (i % 100 == 0)
				r = realloc(r, sizeof(char*)*100);
		}
	}
	else {
		const char delimC[] = "|\n";
		for (token = strtok_r(s, delimC, &rest); token != NULL; token = strtok_r(NULL, delimC, &rest)) {   
			r[i++] = token;
			if (i % 100 == 0)
				r = realloc(r, sizeof(char*)*100);
		}
	}
	return r;
}


static void run_cmd(char **cmd) {
	int es;
	int status;
	int   p[2];
	pid_t pid;
	int   fd_in = 0;
	int   i = 0;
	while (cmd[i] != NULL) {
		pipe(p);
		if ((pid = fork()) < 0) {
			printf("fork failed!\n");
			exit(1);
		}
		else if (pid) {
			waitpid(pid, &status,0);
			close(p[1]);
			fd_in = p[0];
			es = WEXITSTATUS(status);
			++i;
		} 
		else {
			char **args = parse(cmd[i], ARG);
			dup2(fd_in, 0);
			if (cmd[i+1] != NULL)
				dup2(p[1], 1);
			close(p[0]);
			execvp(args[0], args);
			printf("exec* failed\n");
			exit(2);
		}
	}
	return;
}




int main() {
	
	while(1) {
		char *cmd = NULL;
		size_t len = 0;
		printf(" >>>>>>>> ");
		getline(&cmd, &len, stdin);
		char **commands = parse(cmd, CMD);
		run_cmd(commands);
	}
	return 0;
}
