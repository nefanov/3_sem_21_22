#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

char** parse (const char* str_0)
{
	assert (str_0);

	int l = 50;
	char** r = (char**) calloc (l, sizeof (char*));
	char* rest = NULL;
	char* token = NULL;
	char* str = (char*) calloc (strlen (str_0) + 1, sizeof (char));
	if (str)
		strcpy (str, str_0);
	else
		perror ("str is NULL");
	int i = 0;
	const char delim[] = " ,\n";

	for (token = strtok_r (str, delim, &rest); token; token = strtok_r (NULL, delim, &rest))
	{
		r[i++] = token;
		if (i == l)
		{
			l *= 3;
			l /= 2;
			r = (char**) realloc (r, l * sizeof (char*));
			if (!r)
				perror ("r is NULL");
		}
	}
	return r;
}

char** parse_cmd (const char* cmd)
{
	assert (cmd);

	int l = 50;
	char** r = (char**) calloc (l, sizeof (char*));
	char* rest = NULL;
	char* token = NULL;
	char* str = (char*) calloc (strlen (cmd) + 1, sizeof (char));
	if (str)
		strcpy (str, cmd);
	else
		perror ("str is NULL");
	int i = 0;
	const char delim[] = "|\n";
	for (token = strtok_r (str, delim, &rest); token; token = strtok_r (NULL, delim, &rest))
	{
		r[i++] = token;
		if (i == l)
		{
			l *= 3;
			l /= 2;
			r = (char**) realloc (r, l * sizeof (char*));
			if (!r)
				perror ("r is NULL");
		}
	}
	return r;
}

static void run_cmd (char** cmd)
{
	assert (cmd);

	int p[2] = {};
	int fd_in = 0;
	int i = 0;
	pid_t pid;
	while (cmd[i])
	{
		pipe (p);
		if ((pid = fork ()) < 0)
		{
			printf ("Error! Fork failed!\n");
			exit (1);
		}
		else if (pid)
		{
			int status = 0;
			waitpid (pid, &status, 0);
			close (p[1]);
			fd_in = p[0];
	         	i += 1;
		}
		else
		{
			char** args = parse (cmd[i]);
			dup2 (fd_in, 0);
			if (cmd[i + 1])
				dup2 (p[1], 1);
			close (p[0]);
			execvp (args[0], args);
			printf ("Error! Exec* failed!\n");
			exit (2);
		}
	}

	return;
}


int main ()
{
	while (1)
	{
		char* cmd = NULL;
		size_t length = 0;
		printf ("\n >> ");
		getline (&cmd, &length, stdin);
		char** commands = parse_cmd (cmd);
		run_cmd (commands);
	}
	return 0;
}
