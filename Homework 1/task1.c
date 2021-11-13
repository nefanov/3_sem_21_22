//Principle:After string parsing we will get array of substring each of which denotes command. After that we will make a fork and processes will perform this commands.
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

char ** parsing(char* string, char* delim)
{
	char ** com =malloc(sizeof(char*)*100);
	int i = 0;
        for (char *s = strtok(string,delim); s != NULL; s = strtok(NULL, delim))
        {
                com[i] = s;
                i++;
                if (i % 100 == 0)
                	com = realloc(com, sizeof(char*)*100);
        }
        return com;
}

static void run_cmd(char *cmd)
{	
	int i = 0;
	char **com = parsing(cmd, "|\n");
	int p[2];
	int fd_in = 0;
	while (com[i] != NULL) 
	{
		pipe(p);
		const pid_t pid = fork();
		if (pid < 0) {
			printf("fork error\n");
			exit(-1);
		}
		if (pid) {
			int status;
			waitpid(pid, &status,0);
			close(p[1]);
			if (i>0)
				close(fd_in);
			fd_in = p[0];
			i++; 
		} else {
			char **args = parsing(com[i], " ,\n");
			if (i>0)
				dup2(fd_in, 0);
			if (com[i+1] != NULL)
				dup2(p[1], 1);
			close(p[0]);
			execvp(args[0], args);
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
		size_t len = 0;
		getline(&cmd, &len, stdin);
		run_cmd(cmd);
	}
  	return 0;
 }


