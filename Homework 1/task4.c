//Principle: Method of Monte Carlo based on probability that randomly chosen point is located in the right area. We should work with big amount of points that is why we will use threads, which will process points at the same time.
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>
#include <sched.h> 
#include <unistd.h>
#define amount 1000000000
int max_thread = 10;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
double sum = 0;
long int treadpoints;
double f(double x)
{
        return -x*x+4*x;
}

void* goodpoints(void * arg){
	double s = 0;
 	double x;
        double y;
        unsigned int seed = clock();
	for (int i = 0; i < treadpoints; i++) {
		x = 3.0*rand_r(&seed)/RAND_MAX; 
		y = 4.0*rand_r(&seed)/RAND_MAX; 
		if(y-f(x) <= 0) s += 1;
	}
	pthread_mutex_lock(&mutex);
	sum += s;
	pthread_mutex_unlock(&mutex);
}
int main()
{
	FILE* f = fopen("out.txt", "w");
	struct timespec t_start, t_stop;
        for (int i = 1; i <= max_thread; i++) {
              	 treadpoints = amount / i;
              	 clock_gettime( CLOCK_MONOTONIC, &t_start);
       	 pthread_t threads[i];//array of threads
       	 for (int j = 0; j < i; j++) {
        		pthread_create(&threads[j], NULL, goodpoints, NULL);
       	 } 
         	 for (int j = 0; j < i; j++) {
        		pthread_join(threads[j], NULL);
       	 }
       	 clock_gettime( CLOCK_MONOTONIC, &t_stop);
        	pthread_mutex_destroy(&mutex);
        	printf("Value of this function integral= %lf\n", (12*sum/(double)amount) );
        	sum = 0;
        	fprintf(f, "%f\n", ( t_stop.tv_sec - t_start.tv_sec )+(double)(t_stop.tv_nsec-t_start.tv_nsec)/(1000000000.0));

        }
        fclose(f);
        return 0;
}
