#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>

static pid_t pid;

static int byte = 0;
static int bits = 128;

void get_zero( int signo ) {
	bits >> 1;
	
	kill( pid, SIGUSR1 );
}

void get_one( int signo ) {
	byte += bits;
	bits >> 1;

	kill( pid, SIGUSR1 );
}

void empty( int signo ) {
}

int main( int argc, char* argv[] ) {
	sigset_t sset;
	pid_t ppid = getpid();
	
	struct sigaction act_exit;
	struct sigaction act_one;
	struct sigaction act_zero;

  	memset(&act_exit, 0, sizeof( act_exit ));
  	act_exit.sa_handler = exit;
  	sigfillset( &act_exit.sa_mask );
  	sigaction( SIGCHLD, &act_exit, NULL );

	memset( &act_one, 0, sizeof( act_one ) );
    	act_one.sa_handler = get_one;
    	sigfillset( &act_one.sa_mask );
    	sigaction( SIGUSR1, &act_one, NULL );

	memset( &act_zero, 0, sizeof( act_zero ) );
    	act_zero.sa_handler = get_zero;
    	sigfillset( &act_zero.sa_mask );
    	sigaction( SIGUSR2, &act_zero, NULL );

	pid = fork();

	sigaddset( &sset, SIGUSR1 );
	sigaddset( &sset, SIGUSR2 );
	sigaddset( &sset, SIGCHLD );
	sigprocmask( SIG_BLOCK, &sset, NULL );
	sigemptyset( &sset );

	if ( pid == 0 ) {
		int fd;
		char a;
		
		struct sigaction act_empty;
		
		memset( &act_empty, 0, sizeof( act_empty ) );
    		act_empty.sa_handler = empty;
    		sigfillset( &act_empty.sa_mask );
    		sigaction( SIGUSR1, &act_empty, NULL );
		
		fd = open( argv[1], O_RDONLY );

		while ( read( fd, &a, 1 ) > 0 ) {
			for ( int i = 128; i > 0; i >> 1 ) {
				if ( i & a ) {
					kill( ppid, SIGUSR1 );
				}
				else {
					kill( ppid, SIGUSR2 );
				}
				sigsuspend( &sset );
			}
		}
		exit( EXIT_SUCCESS );
	}
	FILE* file = fopen( argv[2], "w" );
	
	do {
		if ( bits == 0 ) {
			write( fileno(file), &byte, 1 );
			byte = 0;
			bits = 128;
		}
		sigsuspend( &sset) ;
	} while( 1 );
	
	exit( EXIT_SUCCESS );
}
