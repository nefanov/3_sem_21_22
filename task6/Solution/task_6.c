#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define PATH_LEN 256
#define MSG_LEN 232
#define SLEEP_TIME 15
#define MAPSLINE_CNT 6

typedef struct MapsLine {
  	char* address;
  	char* perms;
  	char* offset;
  	char* device;
  	char* inode;
  	char* path;
} MapsLine;

static int log_fd;
static FILE* m_file;

typedef struct Config {
	u_char options;         
  	pid_t pid;              
} Config;

Config parse_args(int argc, char* argv[]) {
	Config Conf;
	Conf.options = -1;
	if (argc < 2) {
		printf("Write ./daemon -d for daemon mode and -i for interactive mode\n");
		exit(0);
	}
	if (strcmp(argv[1], "-i") == 0) {
		printf("interactive mode\n");
		Conf.options = 0;
	}
	else if (strcmp(argv[1], "-d") == 0) {
		printf("daemon mode\n");
		Conf.options = 1;
	}
	else {
		printf("Incorrect flag format or value\n");
		exit(1);
	}
	return Conf;
}

char* get_time() {
  	time_t now = time(NULL);
  	char* time_mark = ctime(&now);
  	time_mark[strlen(time_mark)-1] = '\0';

  	return time_mark;
}

int create_log(const char* format, ...) {
	char* log = (char*)(calloc(PATH_LEN, sizeof(char)));
  	char* time = get_time();
	strcat(log, time);
	strcat(log, "\t");
	
	char msg[MSG_LEN];
	va_list log_text;
	va_start(log_text, format);
	vsprintf(msg, format, log_text);
	va_end(log_text);

	strcat(log, msg);
	strcat(log, "\n");

	write(log_fd, log, strlen(log));
	free(log);
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
  	create_log("------->");
  	print_parsed_maps_line(Cur);
  	create_log("--------");
  	print_parsed_maps_line(Next);
  	create_log("<-------");
}

int log_init() {
	const char log_path[] = "../log/log.txt";
  	log_fd = open(log_path,
    	O_CREAT | O_RDWR | O_APPEND,
        S_IRUSR | S_IWUSR);
  	if (log_fd == -1) {
    	perror("Can't open log.txt");
    	return -1;
  	}
  	return 0;
}

void configure_service(Config* Conf) {
	log_fd = fileno(stdout);

	switch(Conf->options & 1) {
		case 0:
			create_log("conf in progress");
			break;
		case 1:
			log_init();
			close(fileno(stdin));
			close(fileno(stdout));
			close(fileno(stderr));

			pid_t pid = setsid();

			umask(0);
			chdir("/");

			sigset_t mask;
			sigfillset(&mask);
			sigprocmask(SIG_SETMASK, &mask, NULL);

			break;
	}

}

MapsLine parse_maps_line(char* line) {
  	MapsLine PML = {NULL, NULL, NULL, NULL, NULL, NULL};

  	int i = 1;
  	char delim[] = " \n";
  	for(char* p = strtok(line, delim); p != NULL; p = strtok(NULL, delim), i++) {
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
      		default:   
             	break;
    	}
  	}

  	return PML;
}

int parse_maps(MapsLine** PML) {
  	char* buf = NULL;
  	size_t len = 0;
  	ssize_t read_sz;

  	size_t PML_sz = 0;
  	int count = 0;

  	while((read_sz = getline(&buf, &len, m_file)) != -1) {
    	PML_sz += sizeof(MapsLine);
    	*PML = realloc(*PML, PML_sz);
    	(*PML)[count] = parse_maps_line(buf);

    	print_parsed_maps_line(&(*PML)[count]);
    	count++;
  	}

  	free(buf);
  	return count;
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

int free_PML(MapsLine** pPML, int num_lines) {
  	MapsLine* PML_arr = *pPML;
  	MapsLine PML;
  	for (int i = 0; i < num_lines; i++) {
    		PML = PML_arr[i];
    		for (int j = 0; j < MAPSLINE_CNT; j++) {
      			switch (j) {
        			case 0:
          				if (PML.address != NULL) {
            				free(PML.address);
          				}
          				break;
        			case 1:
          				if (PML.perms != NULL) {
            				free(PML.perms);
          				}
          				break;
        			case 2:
          				if (PML.offset != NULL) {
            				free(PML.offset);
          				}
          				break;
        			case 3:
          				if (PML.device != NULL) {
            				free(PML.device);
          				}
          				break;
        			case 4:
          				if (PML.inode != NULL) {
            				free(PML.inode);
          				}
          				break;
        			case 5:
          				if (PML.path != NULL) {
            				free(PML.path);
          				}
          				break;
        			default:
          				return -1;
      			}
    		}
  	}
  	free(*pPML);

  	return 0;
}

int PML_swap(MapsLine** Cur, int* cnt_cur, MapsLine** Next, int* cnt_next) {
	MapsLine* tmp = *Cur;
  	*Cur = *Next;
  	*Next = tmp;

  	free_PML(Next, *cnt_cur); 
    
  	*Next = NULL;

  
  	*cnt_cur = *cnt_next;
 	*cnt_next = 0;

  
  	return 0;
}

int open_maps_file(pid_t tr_pid) {
  	char maps_path[PATH_LEN] = "/proc/";
  	char tr_pid_dir[PATH_LEN];

  	sprintf(tr_pid_dir, "%d", tr_pid);
  	strcat(maps_path, tr_pid_dir);
  	strcat(maps_path, "/maps");

  	m_file = fopen(maps_path, "r");
  	if (m_file == NULL) {
    	return -1;
  	} 
  	return 0;
}

int start_service(pid_t pid) {
	open_maps_file(pid);
			
	MapsLine* PML_Cur = NULL;
	MapsLine* PML_Next = NULL;
	int cur, next;

	cur = parse_maps(&PML_Cur);
	
	if (cur == -1) {
		return -1;
	}
	
	fclose(m_file);

	while(1) {
		sleep(SLEEP_TIME);
		open_maps_file(pid);

		next = parse_maps(&PML_Next);
		if (next == -1) {
			return -1;
		}

		PML_diff(&PML_Cur, cur, &PML_Next, next);
    		PML_swap(&PML_Cur, &cur, &PML_Next, &next);
    
    		fclose(m_file);
	}
	free_PML(&PML_Cur, cur);

	return 0;
}

int main(int argc, char* argv[]) {
	pid_t pid;
	Config Conf = parse_args(argc, argv);

	printf("Let's try! Write down some PID: ");
	scanf("%d", &Conf.pid);

	switch(Conf.options & 1) {
		case 0:
			configure_service(&Conf);
			start_service(Conf.pid);
			break;
		case 1:
			pid = fork();
			switch(pid) {
				case -1:
					perror("fail");
					return 1;
				case 0:
					configure_service(&Conf);
					start_service(Conf.pid);
					break;
				default:
					printf("Success");
					break;
			}
			break;
		default:
			printf("Try another args or use instruction");
			return 1;
	}

	return 0;
}
