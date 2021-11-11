#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

int fullcounter = 0;
int N = 1000000000;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long double down = 0;
long double up = 10000;


int under (long double x, long double y);
long double func (long double x);
void *thread (void *args);

/*
 * 1 thread - 42.66 sec
2 threads - 22.53 sec
3 threads - 15.84 sec
4 threads - 11.87 sec
5 threads - 11.45 sec
6 threads - 12.35 sec
7 threads - 11.63 sec
8 threads - 11.43 sec
9 threads - 11.34 sec
10 threads - 11.60 sec
12 threads - 11.28 sec
16 threads - 11.31 sec
32 threads - 11.19 sec
64 threads - 11.13 sec
200 threads - 11.10 sec
1000 threads - 11.12 sec
*
* */

int main () {
	int n;
	scanf ("%d", &n);
	long double a = 0, b = 10;
	pthread_t* id;
	id = (pthread_t*)malloc (n * sizeof (pthread_t));
	long double* section;
	struct timespec tp1, tp2;
	clock_gettime (CLOCK_REALTIME, &tp1);
	int pointnumb = N / n;
	int Nh = N - N % n;
	for (int i = 0; i < n; i++)
	{
		section = (long double *)malloc (3 * sizeof(long double));
		section[0] = a + i * (b - a) / n;
		section[1] = a + (i + 1) * (b - a) / n;
		section[2] = pointnumb;
		pthread_create (&id[i], NULL, thread, section);	
	}
	for (int i = 0; i < n; i++)
	{
		pthread_join (id[i], NULL);
	}
	clock_gettime (CLOCK_REALTIME, &tp2);
	long double time = (tp2.tv_sec - tp1.tv_sec) * 1e9 + tp2.tv_nsec-tp1.tv_nsec;
	printf ("%d %d\n", fullcounter, Nh);
	long double fc = fullcounter;
	printf ("%Lf\n", (b - a) * (up - down) * (fc / Nh));
	printf ("%Lf\n", time / 1e9);
}

long double func (long double x) {
        return 4 * x * x * x + 3 * x * x + 2 * x + 1;
        //integral(4x^3+3x^2+2x+1)=x^4+x^3+x^2+x+C
}

int under (long double x, long double y) {
        if (func(x) >= y)
        {
                return 1;
        }
        return 0;
}

void *thread (void *args) {
        long double * section1 = (long double *) args;
        long double l = section1[0];
        long double r = section1[1];
        int numb = section1[2];
        int counter = 0;
        unsigned int rp = 27;
        for (int i = 0; i < numb; i++)
        {

                long double ran = rand_r(&rp);
                long double ran1 = ran / RAND_MAX;
                long double x = l + ran1 * (r - l);
                ran = rand_r(&rp);
                ran1 = ran / RAND_MAX;
                long double y = down + ran1 * (up - down);
                counter += under(x, y);
        }
        long double hel = counter;
        pthread_mutex_lock(&mutex);
        fullcounter +=counter;
        pthread_mutex_unlock(&mutex);
        pthread_exit (NULL);
}

