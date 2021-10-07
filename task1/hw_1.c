#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


char** parse(const char* s_0, int flag) {
    char** r = malloc(sizeof(char*)*100);
    char *rest = NULL;
    char *token;
    char* s = strdup(s_0);
    int i = 0;
    const char* delim;
    if (flag) {
        delim = " ,\n";
    }
    else {
        delim = "|\n";
    }
    for (token = strtok_r(s, delim, &rest); token != NULL; token = strtok_r(NULL, delim, &rest)) {
        r[i++] = token;
        if (i % 100 == 0)
            r = realloc(r, sizeof(char*)*100);
    }
    return r;
}

static void run_cmd(const char *cmd)
{
  int status;
  const pid_t pid = fork();
  if (pid < 0) {
    printf("fork failed!\n");
  }
  if (pid) {
      waitpid(pid, &status, 0);
      printf("Ret code: %d\n", WEXITSTATUS(status));
      return;
  }
  else {
      char **args = parse(cmd, 1);
      execvp(args[0], args);
      printf("exec* failed\n");
  }
}

int main()
{
  while(1) {
    char *cmd;
    fgets(cmd, 255, stdin);
    int i = 0;
    while (parse(cmd, 0)[i] != NULL) {
        run_cmd(parse(cmd, 0)[i]);
        i++;
    }
  }

  return 0;
 }
