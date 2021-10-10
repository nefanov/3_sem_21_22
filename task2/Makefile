
CC=gcc
DEPS = task2.h
OBJ =  task2_main.o task2_func.o 

LIBS = -lm

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


result_file.c: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)	
	