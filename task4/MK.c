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
#define points 1300000000

long S = 0;
long thread_points;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double f(double x) {
	return x * x;
}

void* cnt() {
	long k = 0;
	unsigned int seed = clock();
	for (long i = 0; i < thread_points; i++) {
		double x = 1.0 * rand_r(&seed) / RAND_MAX;
		double y = 1.0 * rand_r(&seed) / RAND_MAX;
		k += (y < f(x));
	}

	pthread_mutex_lock(&mutex);
	S += k;
	pthread_mutex_unlock(&mutex);

	return NULL;
}

int main(int agrc, char* argv[]) {
	FILE* f;
	f = fopen("res.txt", "w");
	struct timeval start, stop;
	for (int n_threads = 1; n_threads <= 20; n_threads++) {
		S = 0;
		thread_points = points / n_threads;

		pthread_t threads[n_threads];

		for (int i = 0; i < n_threads; i++) {
			pthread_create(&threads[i], NULL, cnt, NULL);
		}

		gettimeofday(&start, NULL);
		
		for (int i = 0; i < n_threads; i++) {
			pthread_join(threads[i], NULL);
		}	

		gettimeofday(&stop, NULL);

		pthread_mutex_destroy(&mutex);

		printf("Integral of f(x) from 0 to 1 is %f\n", 1.0 * S / (thread_points * n_threads));
    	double secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    	fprintf(f, "%f\n", secs);
	}

	fclose(f);
	return 0;
}
