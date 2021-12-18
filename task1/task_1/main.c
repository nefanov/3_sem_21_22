#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#define ARG_MAX 1048576

typedef struct Command {
    int argc;
    char **argv;
} Command;

char **ParsingLine(char *commands, int *num) {
    char **cmds = NULL;

    int i = 0;
    int cmds_sz = 0;
    char delim[] = "|";
    for (char *p = strtok(commands, delim); p != NULL; p = strtok(NULL, delim)) {
        cmds_sz += sizeof(char *);
        cmds = realloc(cmds, cmds_sz);
        cmds[i] = strdup(p);
        i++;
    }
    *num = i;

    return cmds;
}
Command *ParsingCmd(char **cmds, int num) {
    Command *cwa = malloc(num * sizeof(Command));    // commands with arguments

    for (int i = 0; i < num; i++) {
        cwa[i].argc = 0;
        cwa[i].argv = NULL;
    }

    int j;
    char delim[] = " \n";
    for(int i = 0; i < num; i++) {
        j = 0;
        int cwa_sz_i = 0;
        for (char *p = strtok(cmds[i], delim); p != NULL; p = strtok(NULL, delim)) {
            cwa_sz_i += sizeof(char *);
            cwa[i].argv = realloc(cwa[i].argv, cwa_sz_i);
            cwa[i].argv[j] = strdup(p);
            j++;
        }
        cwa_sz_i += sizeof(char *);
        cwa[i].argv = realloc(cwa[i].argv, cwa_sz_i);
        cwa[i].argv[j] = NULL;

        cwa[i].argc = j;
    }

    return cwa;
}
void run(Command *cmds, int n) {
    int fd[2];
    int fd_in = 0;

    for(int i = 0; i < n; i++) {
        if (pipe(fd) < 0) {
            perror("Pipe creation is failed");
            exit(errno);
        }
        const pid_t pid = fork();
        if (pid < 0) {
            perror("fork() unsuccessful");
            exit(errno);
        }

        if (pid > 0) {
            close(fd[1]);
            if (i > 0) {
                close(fd_in);
            }
            fd_in = fd[0];
        } else {
            if (i > 0) {
                dup2(fd_in, 0);
            }
            if (i != n - 1) {
                dup2(fd[1], 1);
            }
            close(fd[0]);

            execvp(cmds[i].argv[0], cmds[i].argv);
            perror("exec* failed");
            exit(errno);
        }
    }
    for(int i = 0; i < n; i++) {
        wait(NULL);
    }
}

int main() {
    char long_cmd[ARG_MAX];

    while(1) {
        printf("$ ");

        scanf("%s", long_cmd);

        int num;
        char **cmds = ParsingLine(long_cmd, &num);
        Command *cmd_with_args = ParsingCmd(cmds, num);

        run(cmd_with_args, num);
    }

    return 0;
}
