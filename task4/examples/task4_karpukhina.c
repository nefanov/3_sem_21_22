#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
double f(double x) {
    return pow(x,4);
}

struct args {
    int count;
    int hits;  
};


void *func(void *arg) {
unsigned int seed = clock();
//	struct random_data * buffer;
//	int32_t * result;
	//lrand48_r(buffer, result);
    struct args *a = (struct args *)arg;
    for(int i = 0; i < a->count; i++) {
	
        double x = rand_r(&seed) / (double)RAND_MAX;
        double y = rand_r(&seed) / (double)RAND_MAX;
        a->hits += y*1.0 <= 1.0*f(x);
    }
    return NULL;
}

int main(int argc, char **argv) {
    const int N = argc > 1 ? atoi(argv[1]) : 2;
    const int COUNT = argc > 2 ? atoi(argv[2]) : 100000;
    pthread_t *threads = (pthread_t *) malloc(N * sizeof(pthread_t));
    struct args *argums = malloc(N * sizeof(struct args));
    for (int i = 0; i < N; i++) {
        argums[i].hits = 0;
        argums[i].count = COUNT / N;
        int code = pthread_create(threads+i, NULL, func, argums+i);
    }
    int hits = 0;
    for(int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
        hits += argums[i].hits;
    }
    printf("hits=%d\n", hits);
    printf("res=%.8f\n", 1.0*hits / COUNT);
    free(threads);
    free(argums);
}

