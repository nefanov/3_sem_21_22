#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

//compilation with the flags: gcc -Ipthread -Im ...
//after: time ./a.out number_of_cores 100000000

struct arguments
	{
	int n_hits;
	int cnt;
	};
	
void *foo(void *argument)
	{
	unsigned int seed = clock();
	struct arguments *arg = (struct arguments *)argument;
	
	for (int i = 0; i < arg->cnt; i++)
		{
		double x, y =rand_r(&seed)/(double)RAND_MAX;
		
		arg->n_hits = arg->n_hits + 1.0*y <= 1.0*(x*x*x*x);
		}
	
	return NULL;	
	}
	
int main (int argc, char **argv)
	{
	const int CNT = 0;
	const int N = 0;
	
	if (argc > 2)
		{
		const int CNT = atoi(argv[2]);
		}
		
	else
		{
		const int CNT = 100000;
		}
	
	if (argc > 1)
		{
		const int N = atoi(argv[1]);
		}
		
	else
		{
		const int N = 2;
		}
		
	pthread_t *thrds = (pthread_t *) malloc(N * sizeof(pthread_t));	
	struct arguments *args = malloc (N * sizeof(pthread_t));
	
	for (int i = 0; i < N; i++)
		{
		args[i].cnt = CNT/N;
		args[i].n_hits = 0;
		
		int pthr_cr = pthread_create(thrds + i, NULL, foo, args + i);
		}
	
	int hit = 0;
	for (int i = 0; i < N; i++)
		{
		pthread_join (thrds[i], NULL);
		
		hit += args[i].n_hits;
		}
	
	printf ("Number of hits is equal to %d\n", hit);
	printf ("The result is equal to %.8f\n", 1.0*hit/CNT);
	
	free(args);
	free(thrds);	
	}	
