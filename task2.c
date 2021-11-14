#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUF_SZ 65532

typedef struct pPipe 
{
	char buf[BUF_SZ];
    	int len;
    	int fd_back[2];
    	int fd_direct[2];
    	int (*read_direct)();
    	void (*write_direct)();
    	void (*read_back)();
    	void (*write_back)();
} Pipe;

int read_direct(Pipe* p)
{
        read(p -> fd_direct[0], &p -> len, sizeof(int));
        int d = read(p -> fd_direct[0], p -> buf, p -> len);
        close(p -> fd_direct[0]);
        return d;
}

void write_direct(Pipe* p)
{
        write(p -> fd_direct[1], &p -> len, sizeof(int));
        write(p -> fd_direct[1], p -> buf, p -> len);
        close(p -> fd_direct[1]);
}

void read_back(Pipe* p)
{
        read(p->fd_back[0], &p -> len, sizeof(int));
        read(p -> fd_back[0], p -> buf, p -> len);
        close(p -> fd_back[0]);
}

void write_back(Pipe* p)
{
        write(p -> fd_back[1], &p -> len, sizeof(int));
        write(p -> fd_back[1], p -> buf, p -> len);
        close(p -> fd_back[1]);
}


Pipe* createPipe()
{
	Pipe* p = malloc(sizeof(Pipe));
    	p -> len = 0;
    	pipe(p -> fd_direct);
    	pipe(p -> fd_back);
    	p -> read_direct = &read_direct;
    	p -> write_direct = &write_direct;
    	p -> read_back = &read_back;
    	p -> write_back = &write_back;
	return p;
}

int main(int argc, char* argv[])
{
	FILE* old_f;
	FILE* new_f;
	pid_t child = fork();
	Pipe* p = createPipe();
	if (argc < 2)
		printf("Incorrect input\n");
	old_f = fopen(argv[1], "rb");
	new_f = fopen("result.txt", "wb");
	struct stat stat_buf;
	stat(argv[1], &stat_buf);
	if(child == 0)
  	{
		while (p -> read_direct(p) != -1)
    		{
			p -> write_back(p);
		}
	}
	else if (child > 0)
  	{
	   	long long int count = 0;	
		while ((p -> len = fread(p -> buf, sizeof(char), BUF_SZ,old_f))) 	
		{
			p -> write_back(p);
			p -> read_back(p);
			fwrite(p -> buf, sizeof(char), p -> len, new_f);
			if (count % 1000 == 0)
      			{
				system("clear");
				printf("Working\n");
			}
			count++;
		}
		system("clear");
		fclose(old_f);
		fclose(new_f);
		free(p);
		printf("Completed\n");
	}
	return 0;
}
