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
long points_per_thread;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double f(double x) {
	return x * x * x + x * x;
}

void* cnt() {
	long in_int = 0;
	unsigned int seed = clock();
	for (long i = 0; i < points_per_thread; i++) {
		double x = 1.0 * rand_r(&seed) / RAND_MAX;
		double y = 1.0 * rand_r(&seed) / RAND_MAX;
		in_int += (y < f(x));
	}

	pthread_mutex_lock(&mutex);
	S += in_int;
	pthread_mutex_unlock(&mutex);

	return NULL;
}

int main(int agrc, char* argv[]) {
	FILE* fp;
	fp = fopen("res.txt", "w");
	struct timeval start, stop;
	for (int n_threads = 1; n_threads <= 20; n_threads++) {
		S = 0;
		points_per_thread = points / n_threads;

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

		printf("Integral of f(x) from 0 to 1 is %f\n", 1.0 * S / (points_per_thread * n_threads));
    	double secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    	fprintf(fp, "%f\n", secs);
	}

	fclose(fp);
	return 0;
}
