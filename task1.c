#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

char** parse(char* str, char delim) {
	char** r = malloc( sizeof( char* )*100 );
	char* rest = NULL;
	char* s_1 = strdup(str);
	int i = 0;
	const char delim_a[] = " ,\n";
    	const char delim_b[] = "|\n";
    	const char* delim_c = (delim == 0) ? delim_a : delim_b;

    	for (char* s = strtok_r(s_1, delim_c, &rest); s != NULL; s = strtok_r(NULL, delim_c, &rest))
	{
        r[i++] = s;
        if (i % 100 == 0)
            r = realloc(r, sizeof(char*)*100);
	}

    return r;
    }

static void run_cmd(char** cmd)
{
	int status;
	int p[2];
	pid_t pid;
	int fd_in = 0;
	int i = 0;
	while (cmd[i] != NULL)
	{
		pipe(p);

		if ((pid = fork()) < 0)
		{
			printf("Fork failed!\n");
			exit(1);
		}

		else if (pid)
		{
			waitpid(pid, &status, 0);
			close(p[1]);
			fd_in =  p[0];
			i++;
		}

		else
		{
			char** args = parse( cmd[i], 0 );
			dup2( fd_in, 0 );
			if ( cmd[i + 1] != NULL )
				dup2( p[1], 1 );
			close( p[0] );
			execvp( args[0], args);
			printf( "Execution error!\n" );
			exit( 2 );
		}
	}

	return;
}

int main()
{
	while (1)
	{
		char* cmd = NULL;
		size_t len = 0;
		printf("\n >> ");
		getline(&cmd, &len, stdin);
		char** cmds = parse(cmd, 1);
		run_cmd(cmds);
	}

	return 0;
}
