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
#define totalpoints 1300000000


long K = 0;
long points_per_thread;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


double f(double x) {
  return x * x * x;
}


void *runner() {
  long in_int = 0;
  unsigned int seed = clock();
  for (long i = 0; i < points_per_thread; i++) {
    double x = 1.0 * rand_r(&seed) / RAND_MAX;
    double y = 1.0 * rand_r(&seed) / RAND_MAX;
    in_int += (y < f(x));
  }

  pthread_mutex_lock(&mutex);
  K += in_int;
  pthread_mutex_unlock(&mutex);
  return 0;
}


int main(int argc, const char *argv[])
{
  struct timeval start, stop;
  FILE *fp;
  fp = fopen("results", "w");
  for (int thread_count = 1; thread_count <= 20; ++thread_count) {
    K = 0;
    points_per_thread = totalpoints / thread_count;

    pthread_t threads[thread_count];

    int i;    
    for (i = 0; i < thread_count; i++) {
      pthread_create(&threads[i], NULL, runner, NULL);
    }

    gettimeofday(&start, NULL);

    for (i = 0; i < thread_count; i++) {
      pthread_join(threads[i], NULL);
    }

    gettimeofday(&stop, NULL);

    pthread_mutex_destroy(&mutex);

    printf("Integral of f(x) from 0 to 1 is %f\n", 1.0 * K / (points_per_thread * thread_count));
    double secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    fprintf(fp, "%f\n", secs);
  }
  fclose(fp);
  return 0;
}