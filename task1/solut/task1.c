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

char **separate_line(char *commands, int *num) {
    char **comands = NULL;

    int i = 0;
    int comands_sz = 0;
    char delim[] = "|";
    for (char *p = strtok(commands, delim); p != NULL; p = strtok(NULL, delim)) {
        comands_sz += sizeof(char *);
        comands = realloc(comands, comands_sz);
        comands[i] = strdup(p);
        i++;
    }
    *num = i;

    return comands;
}
Command *separate_cmd(char **comands, int num) {
    Command *prmtr = malloc(num * sizeof(Command));    // commands with arguments

    for (int i = 0; i < num; i++) {
        prmtr[i].argc = 0;
        prmtr[i].argv = NULL;
    }

    int j;
    char delim[] = " \n";
    for(int i = 0; i < num; i++) {
        j = 0;
        int prmtr_sz_i = 0;
        for (char *p = strtok(comands[i], delim); p != NULL; p = strtok(NULL, delim)) {
            prmtr_sz_i += sizeof(char *);
            prmtr[i].argv = realloc(prmtr[i].argv, prmtr_sz_i);
            prmtr[i].argv[j] = strdup(p);
            j++;
        }
        prmtr_sz_i += sizeof(char *);
        prmtr[i].argv = realloc(prmtr[i].argv, prmtr_sz_i);
        prmtr[i].argv[j] = NULL;

        prmtr[i].argc = j;
    }

    return prmtr;
}
void realizezz(Command *comands, int n) {
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

            execvp(comands[i].argv[0], comands[i].argv);
            perror("exec* failed");
            exit(errno);
        }
    }
    for(int i = 0; i < n; i++) {
        wait(NULL);
    }
}
void Printcomands(Command *comands, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < comands[i].argc; j++) {
            printf("%s ", comands[i].argv[j]);
        }
        printf("\n");
    }
}

int main() {
    char long_cmd[ARG_MAX];

    while(1) {
        printf("$ ");

        scanf("%s", long_cmd);

        int num;
        char **comands = separate_line(long_cmd, &num);
        Command *cmd_with_args = separate_cmd(comands, num);

        realizezz(cmd_with_args, num);
    }


    return 0;
}
