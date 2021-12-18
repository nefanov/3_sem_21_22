#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>



char **parse(char* str, int flag) {
	char del0[] = " ,\n";
	char del1[] = "|\n";
	int i = 0;
	char *saveptr = NULL;
	char **result = malloc(sizeof(char*) * 80);
	if (flag == 1) {
		for (char *token  = strtok_r(str, del1, &saveptr); token != NULL; token = strtok_r(NULL, del1, &saveptr)) {
			result[i] = token;
			i++;
			if (i % 80 == 0) {
				result = realloc(result, sizeof(char*) * 80);
			}
		}
	}
	else {
		for (char *token = strtok_r(str, del0, &saveptr); token != NULL; token = strtok_r(NULL, del0, &saveptr)) {
                	result[i] = token;
                	i++;
                	if (i % 80) {
                        	result = realloc(result, sizeof(char) * 80);
                	}
        	}
	}
	return result;
}

static void run_cmd(char *cmd)
{
	int status;
	const pid_t pid = fork();
	if (pid < 0) {
    		printf("fork failed!\n");
    		return;
  	}
  	if (pid) {
   		waitpid(pid, &status,0);
  		printf("Ret code: %d\n", WEXITSTATUS(status));
   		return;
	} 
  	char **args=parse(cmd,1);
  	execvp(args[0], args);
 	printf("exec* failed\n");
}

int main()
{
 	while(1) {
  		char *cmd;
  		fgets(cmd, 255, stdin);
  		char **parsed_cmd=parse(cmd,0);
  		int i=0;
  		while (parsed_cmd[i]!=NULL) {
     			run_cmd(parsed_cmd[i]);
     			i++;
    		}
}
return 0;
}
