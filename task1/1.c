#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

char ** parse_cmd(char* s, char* delim)
{
	char ** cmds =malloc(sizeof(char*)*20);
	char *p;
	int i = 0;
        for (char *p = strtok(s,delim); p != NULL; p = strtok(NULL, delim))
        {
                cmds[i] = p;
                i++;
                if (i % 100 == 0)
                	cmds = realloc(cmds, sizeof(char*)*20);
        }
        return cmds;
}

static void run_cmd(char *cmd)
{	
	int ec;
	int i = 0;
	char **cmds = parse_cmd(cmd, "|\n");
	int p[2];
	int fd_in = 0;
	while (cmds[i] != NULL) 
	{
		pipe(p);
		const pid_t pid = fork();
		if (pid < 0) {
			printf("fork failed!\n");
			exit(1);
		}
		if (pid) {
			int status;
			waitpid(pid, &status,0);
			close(p[1]);
			if (i>0)
				close(fd_in);
			fd_in = p[0];
			ec = WEXITSTATUS(status);
			i++; 
		} else {
			char **args = parse_cmd(cmds[i], " ,\n");
			if (i>0)
				dup2(fd_in, 0);
			if (cmds[i+1] != NULL)
				dup2(p[1], 1);
			close(p[0]);
			execvp(args[0], args);
			printf("exec* failed\n"); 
			exit(2); 	
		}
	}
	return;
}
  
 

int main()
{
	while(1) {
		char *cmd = NULL;
		size_t len = 0;
		getline(&cmd, &len, stdin);
		run_cmd(cmd);
	}
  	return 0;
 }
 
