#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>
#include <sched.h> 
#include <unistd.h>

#define amount 1000000000
int max_thread = 10;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
double sum = 0;
long int tread_volume;

double f(double x) //Integrated function
{
  return x*x - 4*x + 4;
}

void* counter(void * arg)
{
	double s = 0;
 	double x;
 	double y;
  	unsigned int seed = clock();
	for (int i = 0; i < tread_volume; i++)
  	{
		x = 4.0*rand_r(&seed)/RAND_MAX; 
		y = 4.0*rand_r(&seed)/RAND_MAX; 
		if(y - f(x) <= 0) 
			s += 1;
	}
	pthread_mutex_lock(&mutex);
	sum += s;
	pthread_mutex_unlock(&mutex);
	return 0;
}

int main()
{
	FILE* f = fopen("out.txt", "w");
	struct timespec t_start, t_stop;
	
  	for (int i = 1; i <= max_thread; i++)
  	{
    		tread_volume = amount / i;
    		clock_gettime(CLOCK_MONOTONIC, &t_start);
    		pthread_t threads[i];//array of threads
    		
    		for (int j = 0; j < i; j++)
      			pthread_create(&threads[j], NULL, counter, NULL);
    			 
   		for (int j = 0; j < i; j++)
      			pthread_join(threads[j], NULL);
      			
    	clock_gettime(CLOCK_MONOTONIC, &t_stop);
    	pthread_mutex_destroy(&mutex);
    	printf("Integral of x^2+4x+4 from 0 to 4 = %lf\n", (16*sum/	(double)amount) );
    	sum = 0;
    	fprintf(f, "%f\n", ( t_stop.tv_sec - t_start.tv_sec )+(double)(t_stop.tv_nsec-t_start.tv_nsec)/(1000000000.0));

  	}
  	fclose(f);
  	return 0;
}
