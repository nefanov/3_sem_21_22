//После парсинга получаем набор команд в виде подстрок, которые затем выполняются созданными процессами.
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

char ** parse(char* string, char* delim)
{
	char ** coms = malloc(sizeof(char*)*100);
	int i = 0;
        for (char *s = strtok(string, delim); s != NULL; s = strtok(NULL, delim))
        {
                coms[i] = s;
                i++;
                if (i % 100 == 0)
                	coms = realloc(coms, sizeof(char*)*100);
        }
        return coms;
}

static void run_com(char *com)
{	
	int i = 0;
	char **coms = parse(com, "|\n");
	int p[2];
	int fd_in = 0;
	while (coms[i] != NULL) 
	{
		pipe(p);
		const pid_t pid = fork();
		if (pid < 0) {
			printf("fork error\n");
			exit(-1);
		}
		if (pid) {
			int status;
			waitpid(pid, &status, 0);
			close(p[1]);
			if (i>0)
				close(fd_in);
			fd_in = p[0];
			//int es = WEXITSTATUS(status);  строка для сбора данных о прекратившем работать процессе
			i++; 
		} else {
			char **args = parse(com+i, "|,\n");
			if (i>0)
				dup2(fd_in, 0);
			if (&com[i+1] != NULL)
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
	while(1){
		char *com = NULL;
		size_t len = 0;
		getline(&com, &len, stdin);
		run_com(com);
		}
  	return 0;
 }
