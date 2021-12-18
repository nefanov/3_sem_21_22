#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>

enum Size
{
	Size_of_int_buf = 4500
};

typedef struct pPipe Pipe;

typedef struct op_table Ops;

typedef struct op_table 
{
	size_t (*rcv) (Pipe* self);
	size_t (*snd) (Pipe* self, int);
	void (*delete) (Pipe* self);
} Ops;

typedef struct pPipe
{
	char* name_of_file;
	int fd_direct[2];
	int fd_back[2];
	Ops actions;
	int is_agr;
	pid_t pid_parent;
} Pipe;

size_t send_data_child_pipe (Pipe* self, int is_file_to_pipe)
{
	assert (self);

	int fd;
	char buffer[Size_of_int_buf] = {};
	size_t bytes_sent = 0;
	if (is_file_to_pipe)
	{
		fd = open (self -> name_of_file, O_RDONLY);
	        if (fd == -1)
		{
			perror ("Couldn't open this file\n");
			return 0;
		}
	}
	else
	{
		fd = open ("child.txt", O_CREAT |O_RDWR | O_APPEND, 0666);
		if (fd == -1)
		{
			perror ("Couldn't open this file\n");
			return 0;
		}
	}

	size_t bytes_written = read (fd, buffer, Size_of_int_buf);
	if (bytes_written == -1)
	{
		close (fd);
		perror ("Couldn't read\n");
		return 0;
	}

	while (bytes_written > 0)
	{
		size_t sum = write (self -> fd_back[1], buffer, bytes_written);
		if (sum == -1)
		{
			close (fd);
			perror ("Couldn't write from buffer\n");
			return 0;
		}
		bytes_sent += sum;

		if (bytes_written != Size_of_int_buf)
			break;

		
		bytes_written = read (fd, buffer, Size_of_int_buf);
		if (bytes_written == -1)
		{
			close (fd);
			perror ("Couldn't read\n");
			return 0;
		}
	}

	close (fd);

	return bytes_sent;	
}


size_t receive_data_child (Pipe* self)
{
	assert (self);

	size_t size_of_data = 0;
	char buffer[Size_of_int_buf] = {};
	
	int fd = open ("child.txt", O_CREAT | O_RDWR | O_APPEND, 0666);
	
	int bytes_read = read (self -> fd_direct[0], buffer, Size_of_int_buf);
	if (bytes_read == -1)
	{
		close (fd);
		perror ("Couldn't read\n");
	        return 0;
	}

	while (bytes_read > 0)
	{
		if (write (fd, buffer, bytes_read) != bytes_read)
		{
			close (fd);
			perror ("write error\n");
			return 0;
		}
		size_of_data += bytes_read;

		if (bytes_read != Size_of_int_buf)
			break;
		
		bytes_read = read (self -> fd_direct[0], buffer, Size_of_int_buf);
		if (bytes_read == -1)
		{
			close (fd);
			perror ("Couldn't read\n");
			return 0;
		}
	}

	close (fd);
	return size_of_data;
}

size_t receive_data_parent (Pipe* self)
{
	assert (self);

	pid_t pid = getpid ();

	if (self -> is_agr == 0)
	{
		if (self -> pid_parent == pid)
		{
			close (self -> fd_direct[0]);
			close (self -> fd_back[1]);
	
			self -> is_agr = 1;
		}
		else
		{
			self -> pid_parent = -1;

			close (self -> fd_back[0]);
			close (self -> fd_direct[1]);

			self -> actions.rcv = receive_data_child;
			self -> actions.snd = send_data_child_pipe;

			self -> is_agr = 1;

			return receive_data_child (self);
		}
	}

	size_t size_of_data = 0;
	char buffer[Size_of_int_buf] = {};
	
	int fd = open ("parent.txt", O_CREAT | O_RDWR | O_APPEND, 0666);
	if (fd == -1)
	{
		perror ("Couldn't open this file\n");
		return 0;
	}
	
	size_t bytes_read = read (self -> fd_back[0], buffer, Size_of_int_buf);
	if (bytes_read == -1)
	{
		close (fd);
	
		perror ("Couldn't read\n");
		return 0;
	}

	while (bytes_read > 0)
	{
		if (write (fd, buffer, bytes_read) != bytes_read)
		{
			close (fd);

			perror ("write error\n");
			return 0;
		}

		size_of_data += bytes_read;

		if (bytes_read != Size_of_int_buf)
			break;

		bytes_read = read (self -> fd_back[0], buffer, Size_of_int_buf);
		if (bytes_read == -1)
		{
			close (fd);

			perror ("Couldn't read\n");
			return 0;
		}
	}

	close (fd);
	return size_of_data;
}

size_t send_data_parent (Pipe* self, int is_file_to_pipe)
{
	assert (self);

	pid_t pid = getpid ();
	if (self -> is_agr == 0)
	{
		if (self -> pid_parent == pid)
		{
			close (self -> fd_back[1]);
			close (self -> fd_direct[0]);

			self -> is_agr = 1;
		}
		else 
		{
			self -> pid_parent = -1;

			close (self -> fd_back[0]);
			close (self -> fd_direct[1]);

			self -> actions.rcv = receive_data_child;
			self -> actions.snd = send_data_child_pipe;

			self -> is_agr = 1;

			return send_data_child_pipe (self, is_file_to_pipe);
		}
	}

	int fd;

	if (is_file_to_pipe)
	{
		fd = open (self -> name_of_file, O_RDONLY);
		if (fd == -1)
		{
			perror ("Couldn't open this file\n");
			return 0;
		}
	}
	else
	{
		fd = open ("parent.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
		if (fd == -1)
		{
			perror ("Couldn't open this file\n");
			return 0;
		}
	}	
	
	char buffer[Size_of_int_buf] = {};
	size_t bytes_sent = 0;
	size_t bytes_written = read (fd, buffer, Size_of_int_buf);
	if (bytes_written == -1)
	{
		close (fd);
		perror ("Couldn't read");
		return bytes_sent;
	}

	while (bytes_written > 0)
	{
		size_t sum = write (self -> fd_direct[1], buffer, bytes_written);
		if (sum == -1)
		{
			close (fd);
			perror ("Couldn't write");
			return bytes_sent;
		}

		bytes_sent += sum;

		if (bytes_written != Size_of_int_buf)
			break;

		bytes_written = read (fd, buffer, Size_of_int_buf);
		if (bytes_written == -1)
		{
			close (fd);
			perror ("Couldn't read");
			return bytes_sent;
		}
	}

	close (fd);
	return bytes_sent;
}

void pipe_delete (Pipe* self)
{
	assert (self);

	free (self -> name_of_file);
	
	self -> actions.rcv = NULL;
	self -> actions.snd = NULL;
	self -> actions.delete = NULL;
		
	self -> is_agr = 0;

	if (self -> pid_parent == -1)
	{
		close (self -> fd_back[1]);
		close (self -> fd_direct[0]);
	}
	else
	{
		close (self -> fd_back[0]);
		close (self -> fd_direct[1]);
	}

	self -> pid_parent = -1;
}

int pipe_init (Pipe* self, char* name_of_file)
{
	assert (self);
	assert (name_of_file);
	
	self -> name_of_file = (char*) calloc (strlen (name_of_file) + 1, sizeof (char));
	strcpy (self -> name_of_file, name_of_file);

	int fd1[2], fd2[2];

	if (pipe (fd1) == -1)
	{
		perror ("pipe error\n");
		self -> pid_parent = -1;

		return -1;
	}

	if (pipe (fd2) == -1)
	{
		perror ("pipe error\n");
		self -> pid_parent = -1;
		
		return -1;
	}

	self -> fd_direct[0] = fd1[0];
	self -> fd_direct[1] = fd1[1];
	self -> fd_back[0] = fd2[0];
	self -> fd_back[1] = fd2[1];

	self -> actions.rcv = receive_data_parent;
	self -> actions.snd = send_data_parent;
	self -> actions.delete = pipe_delete;
	
	self -> is_agr = 0;
	self -> pid_parent = getpid ();

	return 0;
}

size_t get_size_of_file (char* name_of_file)
{
	assert (name_of_file);

	struct stat _fileStatbuff;
	
	int fd = open (name_of_file, O_RDONLY);

	if (fd == -1)
	{	
		close (fd);
		perror ("Can't open file\n");
		return -1;
	}
	
	if ((fstat (fd, &_fileStatbuff) != 0) || (!S_ISREG (_fileStatbuff.st_mode)))
	{
		close (fd);
		return -1;
	}
	
	size_t size_of_file = _fileStatbuff.st_size;

	close (fd);
	return size_of_file;
}
	

void echo_test (char* name_of_file)
{
	assert (name_of_file);

	size_t size_of_file = get_size_of_file (name_of_file);
	printf ("File \"%s\" weighs %lu\n\n", name_of_file, size_of_file);

	clock_t start = clock ();
	Pipe pipe;
	pipe_init (&pipe, name_of_file);
	pid_t pid = fork ();
	size_t bytes = 0;
	switch (pid)
	{
		case -1:
			perror ("fork");
			exit (-1);

		case 0:
		        bytes = pipe.actions.rcv (&pipe);
			printf ("Child has received %lu and %lu bytes\n", bytes, size_of_file);
			bytes = pipe.actions.snd (&pipe, 0);
			printf ("Child has send %lu and %lu bytes\n", bytes, size_of_file);
			pipe_delete (&pipe);
			exit (2);

		default:
		        bytes = pipe.actions.snd (&pipe, 1);
			printf ("Parent has send %lu and %lu bytes\n", bytes, size_of_file);
			bytes = pipe.actions.rcv (&pipe);
			printf ("Parent has received %lu and %lu bytes\n", bytes, size_of_file);
			wait (NULL);
			pipe_delete (&pipe);

	}

	clock_t stop = clock ();

	printf ("\nProgram required %lf seconds", (double)(stop - start) / CLOCKS_PER_SEC);
}


int main (int argc, char* argv[])
{
	echo_test (argv[1]);
	return 0;
}
