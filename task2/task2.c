#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE 10000


typedef struct pPipe {
        int fd_parent_child[2]; 
        int fd_child_parent[2]; 
    } Pipe;



    Pipe* constructor () {
        Pipe *t = malloc (sizeof(Pipe));
        pipe (t -> fd_parent_child);
        pipe (t -> fd_child_parent);
        return t;
    }



int main (int argc, char *argv[]) {
    int p1[2], p2[2], status;
    long long int c = 0;
    Pipe* p = constructor ();
    FILE* fp_old;
    FILE* fp_new;
    char buf [BUF_SIZE], b [BUF_SIZE];

    fp_old = fopen (argv[1], "rb");
    fp_new = fopen ("result.txt", "wb");

    struct stat sb;

    stat (argv [1], &sb);
    pid_t child = fork ();

    if (child == 0) {
        while (read (p -> fd_parent_child[0], buf, BUF_SIZE) != -1) {
            close (p -> fd_parent_child[0]);
            write (p -> fd_child_parent[1], buf, BUF_SIZE);
            close (p -> fd_child_parent[1]);              
        }
    }
    if (child > 0){
        int i = 0;
        while (fread (buf, sizeof (char), BUF_SIZE, fp_old)) {            
            write (p -> fd_parent_child[1], buf, BUF_SIZE);
            close (p -> fd_parent_child[1]);
            read (p -> fd_child_parent[0], buf, BUF_SIZE);
            close (p -> fd_child_parent[0]);
            fwrite (buf, sizeof (char), BUF_SIZE, fp_new);
            c += strlen (buf) * 256;
            if (i % 1000 == 0){
                system ("clear");
                printf ("%lld%%\n ", 100 * c / sb.st_size);
            }
            ++i;
        }
    }
    system ("clear");
    printf ("done! file saved as result.txt");
    fclose (fp_old);
    fclose (fp_new);
    free (p);
}
