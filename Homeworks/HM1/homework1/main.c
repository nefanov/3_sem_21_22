#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>


#define MAX_BUF_SIZE 10000
#define MAX_WORDS 30
#define MAX_COMMANDS 30

void freeArray(char **array, unsigned int size)
{
    for (int i = 0; i < size; ++i) {
        free(array[i]);
    }
}

_Bool execCommands(char*** commandSequence, unsigned int commandNumber)
{
    int descriptorOnRead = 0;       //дискриптор, откуда будет читать очередная выполняющаяся комманда

    for (int i = 0; i < commandNumber; ++i) {
        int pipeDescriptors[2];

        if(pipe(pipeDescriptors) < 0)
        {
            printf("Pipe creation failed on the command #%d '%s'\n", commandNumber, commandSequence[i][0]);
            return -1;
        }

        pid_t const pid = fork();

        if(pid < 0)
        {
            printf("'Fork' call failed on the command #%d '%s'\n", commandNumber, commandSequence[i][0]);
            return -1;
        }

        if(pid)                         //родитель
        {
            int status;
            waitpid(pid, &status, 0);

            close(pipeDescriptors[1]);

            if(!WIFEXITED(status))
            {
                printf("Command #%d '%s' has failed", commandNumber, commandSequence[commandNumber][0]);
                return -1;
            }

            if(descriptorOnRead != 0)                   //закрываем предыдущий дескриптор
                close(descriptorOnRead);

            descriptorOnRead = pipeDescriptors[0];
        }
        else{                           //ребёнок
            close(pipeDescriptors[0]);

            if(i != (commandNumber - 1))
                dup2(pipeDescriptors[1], 1);

            dup2(descriptorOnRead, 0);

            int commandStatus = execvp(commandSequence[i][0], commandSequence[i]);

            if(commandStatus < 0)
                exit(-1);

            exit(3);
        }
    }

    close(descriptorOnRead);

    return 1;
}

int main()
{
    char delim[] = " \n";
    char inputBuffer[MAX_BUF_SIZE];        //буффер для записи ввода из консоли
    char* nextCommand[MAX_WORDS];          //буффер для записи очередной команды (max = 30 слов в команде)
    char** comandSequence[MAX_COMMANDS];      //для хранения всего блока команд (max = 20 комманд)

    unsigned int wordNumber = 0;
    unsigned int comandNumber = 0;

    while (1) {
        printf("Enter the command sequence or press Enter to leave: \n");

        if(read(0, inputBuffer, MAX_BUF_SIZE) < 0)
        {
            printf("Unable to read anything\n");
            return 0;
        }

        if(inputBuffer[0] == '\n')
            return 0;

        for(char* word = strtok(inputBuffer, delim); ; word = strtok(NULL, delim))
        {
            if(word == NULL || word[0] == '|' )                      //новая команда(или последняя)
            {
                char** command = (char*)malloc(sizeof (char*) * (wordNumber + 1));

                for (int i = 0; i < wordNumber; ++i) {
                    command[i] = nextCommand[i];
                }
                command[wordNumber] = NULL;                                 //для execvp

                comandSequence[comandNumber] = command;
                comandNumber++;

                for (int i = 0; i < MAX_WORDS; ++i) {
                    nextCommand[i] = NULL;
                }
                wordNumber = 0;

                if(word != NULL)
                    continue;
                else
                    break;
            }

            int wordLen = strlen(word);
            char* newWord = (char*)malloc(sizeof (char) * wordLen);
            strcpy(newWord, word);

            nextCommand[wordNumber] = newWord;
            wordNumber++;
        }

        _Bool result = execCommands(comandSequence, comandNumber);

        for (int i = 0; i < comandNumber; ++i) {                            //освобождаем память
            char** p = comandSequence[i];
            int j = 0;

            while (p[j] != NULL)                                            //каждое слово
                free(p[j++]);

            free(p);                                                        //каждый указатель на массив слов
        }

        for (int i = 0; i < MAX_COMMANDS; ++i) {
            comandSequence[i] = 0;
        }

        for (int i = 0; i < MAX_BUF_SIZE; ++i) {
            inputBuffer[i] = 0;
        }

        comandNumber = 0;

    }


    return 0;
}
