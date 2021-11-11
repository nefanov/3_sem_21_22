#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#define buf_size 65532

//----------------------------structures--------------------------------------------------

typedef struct op_table sOp_t;
typedef struct pPipe sPipe;

typedef struct op_table
	{
	size_t (*rcv)(sPipe *self);
	size_t (*snd)(sPipe *self);
	} sOp_t;
	
typedef struct pPipe
	{
	int len;
	int fd_direct[2]; 
	int fd_back[2];   
	char buf[buf_size];
	void (*w_direct)();
	void (*w_back)();
	void (*r_back)();
	int (*r_direct)();
	} sPipe;	

//----------------------------prototypes--------------------------------------------------
	
void write_direct (sPipe *p);
void write_back   (sPipe *p);
void read_back    (sPipe *p);
int  read_direct  (sPipe *p);
sPipe *constructPipe();	
	
//----------------------------main--------------------------------------------------

int main (int argc, char *argv[])
	{
	long long int current = 0;
	
	sPipe *p = constructPipe();
	
	FILE *fp_new;
	FILE *fp_old;
	
	fp_old = fopen ("file.txt", "r");
	fp_new = fopen ("result.txt", "w");
	
	pid_t child = fork();
	
	if (child == 0)
		{
		while (p->r_direct(p) != -1)
			{
			p->w_back(p);
			}
		}
	
	if (child > 0)
		{
		int i = 0;
		while ((p->len = fread(p->buf, sizeof(char), buf_size, fp_old)))
			{
			p->w_direct(p);
			p->r_back(p);
			
			fwrite (p->buf, sizeof(char), p->len, fp_new);
			
			current += p->len;
			i++;
			}
			
		system("clear");
		printf("Already done! The file was saved as result");
		fclose(fp_old);
		free(p);	
		}
		
	return 0;
	}
	
//----------------------------functions--------------------------------------------------		

void w_direct (sPipe *p)
	{
	write(p->fd_direct[1], &p->len, sizeof(int));
	write(p->fd_direct[1], p->buf, p->len);
	close(p->fd_direct[1]);
	}
	
void w_back (sPipe *p)
	{
	write(p->fd_back[1], &p->len, sizeof(int));
	write(p->fd_back[1], p->buf, p->len);
	close(p->fd_back[1]);
	}
	
void r_back (sPipe *p)
	{
	read(p->fd_back[0], &p->len, sizeof(int));
	read(p->fd_back[0], p->buf, p->len);
	close(p->fd_back[0]);
	}

int r_direct (sPipe *p)
	{
	read(p->fd_direct[0], &p->len, sizeof(int));
	int result = read(p->fd_direct[0], p->buf, p->len);
	close(p->fd_direct[0]);
	return result;
	}	
	
sPipe *constructPipe()
	{	
	sPipe *p = malloc(sizeof(sPipe));
	
	pipe(p->fd_back);
	pipe(p->fd_direct);
	
	p->w_back   = &w_back;
	p->w_direct = &w_direct;
	p->r_back   = &r_back;
	p->r_direct = &r_direct;
	
	p->len = 0;
	
	return p;
	}	
