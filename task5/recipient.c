#define _GNU_SOURCE
#include <signal.h> //signal kill
#include <stdio.h> //printf scanf fopen fwrite fclose
#include <stdlib.h> //malloc
#include <string.h> //memcpy memset
#include <unistd.h> //getpid

unsigned char* data = NULL;
int size = 0;
int bitnum = 0;
int sender_pid;
char* filename = "output.txt";

void alloc() { //allocate more memory if needed
    if(data == NULL) { //if memory hasn't been allocated yet, alloc 1 byte
        data = malloc(1);
        size = 1;
    }
    if(bitnum / 8 < size) //if all bits fit, return
        return;
    unsigned char* buff = data;
    data = malloc(size * 2); //allocate twice as much
    memcpy(data, buff, size); //copy all data
    free(buff); //free previously allocated memory
    size *= 2;    
}
void set_zero(int ind) {
    alloc();
    data[bitnum / 8] &= (1 << (bitnum % 8)) ^ 0xFF; //set the next bit to 0
    bitnum++;
    kill(sender_pid, SIGUSR1); //respond to get the next bit
}
void set_one(int ind) {
    alloc();
    data[bitnum / 8] |= (1 << (bitnum % 8)); //set masked bit to 1
    bitnum++;
    kill(sender_pid, SIGUSR1); //respond to get the next bit
}
void save_and_exit() {

    FILE* file = fopen(filename, "wb"); //open a file in write-binary mode to dump the data
    fwrite(data, 1, bitnum / 8, file); //dump the data
    fclose(file); //close the file

    printf("Data written to %s\n", filename);
    exit(0); //die
}
int main(int argc, char** argv) {
    if(argc > 1)
        filename = argv[1]; //if a custom filename is passed, assign it

    printf("Recipient pid: %d\nSender pid: ", getpid());
    scanf("%d", &sender_pid); //read sender's pid
    signal(SIGUSR1, set_zero);
    signal(SIGUSR2, set_one);
    signal(SIGINT, save_and_exit); //assign signal handlers
    while(1) sleep(1); //i literally can't be bothered to come up with a better solution
}
