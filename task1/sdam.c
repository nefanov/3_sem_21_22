#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>




char** parse_arg(char* s_0);
char** parse_cmd(char* s_0);
static void run_cmd(char **cmd);


int main()
{

        while(1) {
                char *cmd = NULL;
                size_t len = 0;
                printf("\n Enter command\n ");
                getline(&cmd, &len, stdin);
                char **commands = parse_cmd(cmd);
                run_cmd(commands);
        }
        return 0;
}
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\\

char** parse_arg(char* s_0)
{
        char** r = malloc(sizeof(char*)*100);
        char *forstr = NULL;
        char *token;
        char* s = strdup(s_0);
        int i = 0;
        const char delim[] = " ,\n";
        for (token = strtok_r(s, delim, &forstr); token != NULL; token = strtok_r(NULL, delim, &forstr)) {
                r[i++] = token;
                if (i % 100 == 0)
                        r = realloc(r, sizeof(char*)*100);
        }
        return r;
}

static void run_cmd(char **cmd)
{
        int es;
        int status;
        int   p[2];
        pid_t pid;
        int   fd_in = 0;
        int   i = 0;
        while (cmd[i] != NULL) {
                pipe(p);
                if ((pid = fork()) < 0) {
                        printf("fork failed!\n");
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
                        char **args = parse_arg(cmd[i]);
                        dup2(fd_in, 0);
                        if (cmd[i+1] != NULL)
                                dup2(p[1], 1);
                        close(p[0]);
                        execvp(args[0], args);
                        printf("exec* failed\n");
                        exit(2);
                }
        }
        return;
}

char** parse_cmd(char* s_0)
{
        char** r = malloc(sizeof(char*)*100);
        char *forstr = NULL;
        char *token;
        char* s = strdup(s_0);
        int i = 0;
        const char delim[] = "|\n";
        for (token = strtok_r(s, delim, &forstr); token != NULL; token = strtok_r(NULL, delim, &forstr)) {
                r[i++] = token;
                if (i % 100 == 0)
                        r = realloc(r, sizeof(char*)*100);
        }
        return r;
}



