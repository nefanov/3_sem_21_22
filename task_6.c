#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>


#define PATH_LEN 256
#define TIME_MARK_LEN 24
#define MSG_LEN (PATH_LEN - TIME_MARK_LEN)
#define SLEEP_TIME 10


struct MapsLine;
typedef struct MapsLine MapsLine;

FILE *log_fd;

int start_log();
char* get_time();
void create_log(const char* format, ...);
void print_parsed_maps_line(MapsLine* PML);
void print_diff_PML(MapsLine* Cur, MapsLine* Next);


typedef struct MapsLine {
  char* address;
  char* perms;
  char* offset;
  char* device;
  char* inode;
  char* path;
} MapsLine;

static FILE* maps_file;

MapsLine parse_maps_line(char* line);
int parse_maps(MapsLine** PML);
int PML_diff(MapsLine** Cur, int cnt_cur, MapsLine** Next, int cnt_next);
int PML_swap(MapsLine** Cur, int* cnt_cur, MapsLine** Next, int* cnt_next);
int open_maps_file(pid_t tr_pid);
int start_service(pid_t tr_pid);



int start_log() {
  log_fd = fopen("log.txt", "w");
  printf("opened\n");
  fprintf(log_fd, "opened\n");
  return 0;
}


char* get_time() {
  time_t now = time(NULL);
  char* time_mark = ctime(&now);
  time_mark[strlen(time_mark)-1] = '\0';
  return time_mark;
}


void create_log(const char* format, ...) {
  char* log = (char*)(calloc(PATH_LEN, sizeof(char)));
  char* time_mark = get_time();
  strcat(log, time_mark);
  strcat(log, "\t");
  va_list ptr;
  va_start(ptr, format);
  char msg[MSG_LEN];
  vsprintf(msg, format, ptr);
  va_end(ptr);
  strcat(log, msg);
  strcat(log, "\n");
  fprintf(log_fd, log);
}


void print_parsed_maps_line(MapsLine* PML) {
  char msg[MSG_LEN] = "\0";
  if (PML->address != NULL) {
    strcat(msg, PML->address);
    strcat(msg, " ");
  }
  if (PML->perms != NULL) {
    strcat(msg, PML->perms);
    strcat(msg, " ");
  }
  if (PML->offset != NULL) {
    strcat(msg, PML->offset);
    strcat(msg, " ");
  }
  if (PML->device != NULL) {
    strcat(msg, PML->device);
    strcat(msg, " ");
  }
  if (PML->inode != NULL) {
    strcat(msg, PML->inode);
    strcat(msg, " ");
  }
  if (PML->path != NULL) {
    strcat(msg, PML->path);
  }
  create_log(msg);
}

void print_diff_PML(MapsLine* Cur, MapsLine* Next) {
  create_log("Old: ");
  print_parsed_maps_line(Cur);
  create_log("New: ");
  print_parsed_maps_line(Next);
}

static FILE* maps_file;

int start_service(pid_t tr_pid) {
  open_maps_file(tr_pid);
  MapsLine* PML_Cur = NULL;
  MapsLine* PML_Next = NULL;
  int num_lns_cur, num_lns_next;
  num_lns_cur = parse_maps(&PML_Cur);
  fclose(maps_file);
  while(1) {
    sleep(SLEEP_TIME);
    open_maps_file(tr_pid);    
    num_lns_next = parse_maps(&PML_Next);    
    PML_diff(&PML_Cur, num_lns_cur, &PML_Next, num_lns_next);
    PML_swap(&PML_Cur, &num_lns_cur, &PML_Next, &num_lns_next);    
    fclose(maps_file);
  }
  return 0;
}


int open_maps_file(pid_t tr_pid) {
  char maps_path[PATH_LEN] = "/proc/";
  char tr_pid_dir[PATH_LEN];
  sprintf(tr_pid_dir, "%d", tr_pid);
  strcat(maps_path, tr_pid_dir);
  strcat(maps_path, "/maps");
  maps_file = fopen(maps_path, "r");
  return 0;
}


int parse_maps(MapsLine** PML) {  
  char* buf = NULL;
  size_t len = 0;
  ssize_t read_sz;
  size_t PML_sz = 0;
  int count = 0;
  while((read_sz = getline(&buf, &len, maps_file)) != -1) {
    PML_sz += sizeof(MapsLine);
    *PML = realloc(*PML, PML_sz);
    (*PML)[count] = parse_maps_line(buf);
    //print_parsed_maps_line(&(*PML)[count]);
    count++;
  }
  return count;
}


MapsLine parse_maps_line(char* line) {
  MapsLine PML = {NULL, NULL, NULL, NULL, NULL, NULL};
  int i = 1;
  char delim[] = " \n";
  for(char* p = strtok(line, delim);
      p != NULL;
      p = strtok(NULL, delim), i++) {
    switch(i) {
      case 1:
        PML.address = strdup(p);
        break;
      case 2:
        PML.perms = strdup(p);
        break;
      case 3:
        PML.offset = strdup(p);
        break;
      case 4:
        PML.device = strdup(p);
        break;
      case 5:
        PML.inode = strdup(p);
        break;
      case 6:
        PML.path = strdup(p);
        break;
    }
  }
  return PML;
}



int PML_diff(MapsLine** Cur, int cnt_cur, MapsLine** Next, int cnt_next) {
  MapsLine* pCur = *Cur;
  MapsLine* pNext = *Next;  
  for(int i = 0; i < cnt_next && i < cnt_cur; i++) {
    if (pCur[i].address != NULL && pNext[i].address != NULL) {
      if (strcmp(pCur[i].address, pNext[i].address) != 0) {
        print_diff_PML(&pCur[i], &pNext[i]);
        continue;
      }
    }
    if (pCur[i].perms != NULL && pNext[i].perms != NULL) {
      if (strcmp(pCur[i].perms, pNext[i].perms) != 0) {
        print_diff_PML(&pCur[i], &pNext[i]);
        continue;
      }
    }
    if (pCur[i].offset != NULL && pNext[i].offset != NULL) {
      if (strcmp(pCur[i].offset, pNext[i].offset) != 0) {
        print_diff_PML(&pCur[i], &pNext[i]);
        continue;
      }
    }
    if (pCur[i].device != NULL && pNext[i].device != NULL) {
      if (strcmp(pCur[i].device, pNext[i].device) != 0) {
        print_diff_PML(&pCur[i], &pNext[i]);
        continue;
      }
    }
    if (pCur[i].inode != NULL && pNext[i].inode != NULL) {
      if (strcmp(pCur[i].inode, pNext[i].inode) != 0) {
        print_diff_PML(&pCur[i], &pNext[i]);
        continue;
      }
    }
    if (pCur[i].path != NULL && pNext[i].path != NULL) {
      if (strcmp(pCur[i].path, pNext[i].path) != 0) {
        print_diff_PML(&pCur[i], &pNext[i]);
        continue;
      }
    }
  }
  return 0;
}

int PML_swap(MapsLine** Cur, int* cnt_cur, MapsLine** Next, int* cnt_next) {
  MapsLine* tmp = *Cur;
  *Cur = *Next;
  *Next = tmp;
  *Next = NULL;
  *cnt_cur = *cnt_next;
  *cnt_next = 0;
  return 0;
}


int main() {
  pid_t pid1;//make demon

  pid1 = fork();

  if (pid1 < 0)
  exit(EXIT_FAILURE);

  if (pid1 > 0)
  exit(EXIT_SUCCESS);

  if (setsid() < 0)
  exit(EXIT_FAILURE);

  start_log();
  pid_t pid;
  printf("Enter PID: ");
  scanf("%d", &pid);  
  start_service(pid);
  return 0;
} 