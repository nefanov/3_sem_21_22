#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "main_sm.h"

// Return ptr to sem_id or NULL if error occurred
int *my_sem_open(const char *path, short sem_sz);     // Opening semaphore
int my_sem_close(int *sem_id);          // Closing semaphore
int my_sem_remove(const char *path);    // Removing semaphore

int my_sem_post(const int *sem_id);     // Increasing semaphore value
int my_sem_wait(const int *sem_id);     // Decreasing semaphore value