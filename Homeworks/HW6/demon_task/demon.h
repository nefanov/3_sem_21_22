#ifndef DEMON_H

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <utime.h>
#include <signal.h>

void sigHandler(int sigNum);                                               //для связи с демоном

void SearchDirectory(const char *name, char* dirBase, int benchmark);

short copyFile(char*, char*, _Bool);

short diffTest(char* absoluteTargetPath, char* fileName) ;    //запуск diff

_Bool isTextFile(char* fileName);

void demonWork();                                             //основная функция бэкапа

short init(char* configFile, _Bool isDemon);

void tryLater();

#endif // DEMON_H
