#ifndef PROGRAMS_STARTER_H
#define PROGRAMS_STARTER_H

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SMALL_FILE_NAME "./small_size_file.txt"
#define MEDIUM_FILE_NAME "./medium_size_file.txt"
#define BIG_FILE_NAME "./big_size_file.txt"

#define SMALL_LOGFILE_NAME "./small_size_logfile.txt"
#define MEDIUM_LOGFILE_NAME "./medium_size_logfile.txt"
#define BIG_LOGFILE_NAME "./big_size_logfile.txt"

double* executePrograms();

#endif // PROGRAMS_STARTER_H
