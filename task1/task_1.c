#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>




char** parse_arg(char* arg)
{

	char** r = malloc(sizeof(char*)*100);
	char *rest = NULL;
	char *tkn;
	char* s = strdup(arg);
	int i = 0;
	const char delim[] = " ,\n";
	for (tkn = strtok_r(s, delim, &rest); tkn != NULL; tkn = strtok_r(NULL, delim, &rest)) {   
		r[i++] = tkn;
		if (i % 100 == 0)
			r = realloc(r, sizeof(char*)*100);
	}
	return r;
}

char** parse_cmd(char* cmd)
{
	char** r = malloc(sizeof(char*)*100);
	char *rest = NULL;
	char *tkn;
	char* s = strdup(cmd);
	int i = 0;
	const char delim[] = "|\n";
	for (tkn = strtok_r(s, delim, &rest); tkn != NULL; tkn = strtok_r(NULL, delim, &rest)) {   
		r[i++] = tkn;
		if (i % 100 == 0)
			r = realloc(r, sizeof(char*)*100);
	}
	return r;
}

static void run_cmd(char **cmd_s)
{
	int es;
	int status;
	int   p[2];
	pid_t pid;
	int   fd_in = 0;
	int   i = 0;
	while (cmd_s[i] != NULL) {
		pipe(p);
		if ((pid = fork()) < 0) {
			printf("fork failed\n");
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
			char **arg = parse_arg(cmd_s[i]);
			dup2(fd_in, 0);
			if (cmd_s[i+1] != NULL)
				dup2(p[1], 1);
			close(p[0]);
			execvp(arg[0], arg);
			printf("exec failed\n");
			exit(2);
		}
	}
	return;
}





int main()
{

	while(1) {
		char *cmd = NULL;
		size_t l = 0;
		printf("\n >> ");
		getline(&cmd, &l, stdin);
		char **cmds = parse_cmd(cmd);
		run_cmd(cmds);
	}
	return 0;
} 