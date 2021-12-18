#pragma once

#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "sem_sm.h"

#define INDEX_NUM 3
#define SHM_SZ 64
#define BUF_SZ (SHM_SZ - INDEX_NUM * sizeof(size_t))

typedef struct smphr_id {
  int *empty;
  int *mutex;
  int *full;
} Semid;
typedef struct SharedMemory {
  char *ptr;
  size_t *status;  // 1-processing; 0-EOF
  size_t *s_ind;
  size_t *r_ind;
  char *buf;
} SharedMemory;

Semid *smphrsInit(char *argv[]);
void smphrRemove(Semid *sem, char *argv[]);

void *getaddr(const char *path, size_t shm_sz);
SharedMemory *SharedMemoryInit(const char *path);
void SharedMemoryRemove(SharedMemory *Shmem);

void send(char *argv[], SharedMemory *Shmem, Semid *sem);
void receive(char *argv[], SharedMemory *Shmem, Semid *sem);