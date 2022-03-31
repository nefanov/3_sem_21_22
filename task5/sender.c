#define _GNU_SOURCE
#include <signal.h> //kill sigwait sigset
#include <stdio.h> //printf scanf
#include <unistd.h> //getpid
#include <stdlib.h> //malloc

int main(int argc, char** argv) {
    int rec_pid;
    char* filename = "input.txt";
    if(argc > 1)
        filename = argv[1]; //if a custom filename is provided, use it
    printf("Sender pid: %d\nRecipient pid: ", getpid());
    scanf("%d", &rec_pid); //get recipient's pid
    printf("Starting the transmission..\n");
    fflush(stdout); //the following code sometimes causes stdout lag for some reason so i just flush it all now
    FILE* file = fopen(filename, "r"); //open the file in read-binary mode to get data
    char* data = malloc(0x100000); //allocate a mb of mem, i can't be arsed to do any checks so let's assume its always enough
    int size = fread(data, 1, 0x100000, file); //read the data and save its size
    fclose(file);
    printf("%i\n", size);
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL); //block SIGUSR1 to be able to sigwait for it later
    
    for(int i = 0; i < size * 8; i++) {
        if(data[i / 8] & (1 << (i % 8)))
            kill(rec_pid, SIGUSR2); //send a one
        else
            kill(rec_pid, SIGUSR1); //send a zero
        int s;
        sigwait(&set, &s); //wait for response
    }
    kill(rec_pid, SIGINT); //send a terminator
    
}
