#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <sched.h> 
#include <unistd.h>
#include <math.h>
#define points 993993993

long S = 0;
long p; //thread points

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

double f(double x) {
	return sin(x);
}

void* kn() {
	long k = 0;
	unsigned int t = clock();
	for (long i = 0; i < p;i++) {
		double x = 1.0 * rand_r(&t) / RAND_MAX;
		double y = 1.0 * rand_r(&t) / RAND_MAX;
		k += (y < f(x));
	}
	pthread_mutex_lock(&m);
	S += k;
	pthread_mutex_unlock(&m);
	return NULL;
}

int main(int agrc, char* argv[]) {
	FILE* z;
	z = fopen("res.txt", "w");
	struct timeval start, stop;
	for (int num = 1; num <= 16; num++) {
		S = 0;
		p = points / num;
		pthread_t threads[num];
		for (int i = 0; i < num; i++) {
			pthread_create(&threads[i], NULL, kn, NULL);
		}
		gettimeofday(&start, NULL);
		for (int i = 0; i < num; i++) {
			pthread_join(threads[i], NULL);
		}	
		gettimeofday(&stop, NULL);
		pthread_mutex_destroy(&m);
		printf("VALUE: %f\n", 1.0 * S / (p * num));
    	double secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    	fprintf(z, "%f\n", secs);
	}
	fclose(z);
	return 0;
}