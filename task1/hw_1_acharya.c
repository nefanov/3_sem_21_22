#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

char **parsing(char *str_init, int flag)
	{
	char** result = malloc(sizeof(char*)*100);
	char *rest = NULL
	char *str = strdup(str_init);
	char *token;
	
	int i = 0;
	if (flag == 0)
		{
		const char deliminator_null[] = " ,\n";
		
		for(token = strtok_r(str, deliminator_null, &rest); token != NULL; token = strtok_r (NULL, deliminator_null, &rest))
			{
			result[i++] = token;
			
			if (i % 100 == 0) result = realloc(result, sizeof(char*)*100);
			}
		}
	else
		{
		const char deliminator_one[]  = " |\n";
		
		for(token = strtok_r(str, deliminator_one, &rest); token != NULL; token = strtok_r (NULL, deliminator_one, &rest))
			{
			result[i++] = token;
			
			if (i % 100 == 0) result = realloc(result, sizeof(char*)*100);
			}
		}
		
	return result;
	}

static void command_exec (char **cmd)
	{
	int status;
	int i = 0;
	pid_t pid;
	int process[2];
	int exsts;
	int fd_in = 0;
	
	while (cmd[i] != NULL)
		{
		pipe(process);
		
		if ((pid = fork()) < 0)
			{
			printf ("Sorry, fork failed :(\n");
			exit(1)
			}
		else if (pid)
			{
			waitpid (pid, &status, 0);
			close(process[1]);
			
			fd_in = process[0];
			exsts = WEXITSTATUS(status);
			
			++i;
			}
		else 
			{
			char** args = parsing(cmd[i], 0);
			
			dup2 (fd_in, 0);
			if (cmd[i + 1] != NULL) dup2(process[1], 1);
				close(process[0]);
				execvp (args[0], args);
				
				printf ("Sorry, execution failed\m");
				exit(2);
			}
		}
	}

int main()
	{
	while (true):
		{
		char* cmd = NULL;
		
		size_t lenght = 0;
		
		getline (&cmd, &len, stdin);
		
		char **the_commands = parsing (cmd, 1);
		
		command_exec(the_commands);
		}
	
	return 0;
	}		
		
		
		
		
