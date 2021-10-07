#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

char **parse(const char *s_0, int flag)
{
  const char *delim;
  if (flag)
  {
    delim = " ,\n";
  }
  else
  {
    delim = "|\n";
  }

  int i = 0;
  char **r = malloc(sizeof(char*) *100);
  char *p;
  char *rest = NULL;
  char *s = strdup(s_0);

  for (p = strtok_r(s, delim, &rest); p != NULL; p = strtok_r(NULL, delim, &rest))
  {
    r[i++] = p;
    if (i % 100 == 0)
      r = realloc(r, sizeof(char*) *100);
  }

  return r;
}

static void run_cmd(const char *cmd)
{

  const pid_t pid = fork();
  if (pid < 0)
  {
    printf("fork() error \n");
  }
  if (pid)
  {
    int status;
    waitpid(pid, &status, 0);
    printf("Ret code: %d\n", WEXITSTATUS(status));
  }
  else
  {
    char **args = parse(cmd, 1);
    execvp(args[0], args);
    printf("exec* failed\n");
  }
}

int main()
{
  while (1)
  {
    char *cmd;
    printf(">>");
    fgets(cmd, 255, stdin);
    char **cmd_parsed = parse(cmd, 0);
    int i = 0;
    while (cmd_parsed[i] != NULL)
    {
      run_cmd(cmd_parsed[i]);
      i++;
    }
  }
  return 0;
}