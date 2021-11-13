//Интеграл вычисляется методом Монте-Карло(случайных бросков в область графика)
//Повышение скорости достигается за счет того, что каждый поток бросает свои точки

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
int max_thread = 20;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
double sum = 0;
long int threadpoints;
double f(double x)
{
        return 3*x*x+4*x*x*x;
}

void* goodpoints(void * arg){
	double s = 0;
 	double x;
        double y;
        unsigned int seed = clock();
	for (int i = 0; i < threadpoints; i++) {
		x = 1.0*rand_r(&seed)/RAND_MAX; 
		y = 7.0*rand_r(&seed)/RAND_MAX; 
		if(y-f(x) <= 0) s += 1;
	}
	pthread_mutex_lock(&mutex);
	sum += s;
	pthread_mutex_unlock(&mutex);
}
int main()
{
	FILE* f = fopen("Timesthreads.txt", "w");
	struct timespec t_start, t_stop;
        for (int i = 1; i <= max_thread; i++) {
              	 threadpoints = amount / i;
              	 clock_gettime( CLOCK_MONOTONIC, &t_start);
       	 pthread_t threads[i];//array of threads
       	 for (int j = 0; j < i; j++) {
        		pthread_create(&threads[j], NULL, goodpoints, NULL);
       	 } 
         	 for (int j = 0; j < i; j++) {
        		pthread_join(threads[j], NULL);
       	 }
       	 clock_gettime(CLOCK_MONOTONIC, &t_stop);
        	pthread_mutex_destroy(&mutex);
        	printf("Integral counted, value = %lf\n", (7*sum/(double)amount) );
        	sum = 0;
        	fprintf(f, "%f\n", ( t_stop.tv_sec - t_start.tv_sec )+(double)(t_stop.tv_nsec-t_start.tv_nsec)/(1000000000.0));

        }
        fclose(f);
        return 0;
}