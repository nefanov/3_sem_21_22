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
#define points 1000000000
int threads_max = 10;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
double sum = 0;
long int ppt;
double func(double x)
{
        return cos(x);
}
 
 void* counter(void * arg){
 	double s = 0;
 	double x;
        double y;
        unsigned int seed = clock();
	for (int i = 0; i < ppt; i++) {
		x = (double)rand_r(&seed)/RAND_MAX; //генерируем x в интервале [0,1]
		y = (double)rand_r(&seed)/RAND_MAX; //генерируем y в интервале [0,1]
		if(fabs((double)y)-func(x) < 0) s += 1;
	}
	pthread_mutex_lock(&mutex);
	sum += s;
	pthread_mutex_unlock(&mutex);
 }
int main()
{
	FILE* f = fopen("out.txt", "w");
	struct timespec t_start, t_stop;
        for (int i = 1; i <=threads_max; i++) {
       	 pthread_t threads[i];
      	 	 ppt = points / i;
       	 for (int j = 0; j < i; j++) {
        		pthread_create(&threads[j], NULL, counter, NULL);
       	 } 
       	 clock_gettime( CLOCK_MONOTONIC, &t_start);
         	 for (int j = 0; j < i; j++) {
        		pthread_join(threads[j], NULL);
       	 }
       	 clock_gettime( CLOCK_MONOTONIC, &t_stop);
        	pthread_mutex_destroy(&mutex);
        	printf("Result= %lf\n", (sum/(double)points) );
        	sum = 0;
        	fprintf(f, "%f\n", ( t_stop.tv_sec - t_start.tv_sec )+(double)(t_stop.tv_nsec-t_start.tv_nsec)/(1000000000.0));
        	
        }
        fclose(f);
        return 0;
}
