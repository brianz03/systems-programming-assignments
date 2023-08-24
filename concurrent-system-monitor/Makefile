CC = gcc
CCFLAGS = -Wall 
a3: stats_functions.o main.o
	$(CC) $^ -o a3

%.o: %.c stats_functions.h
	$(CC) $(CCFLAGS) $< -c

.PHONY: clean
clean:
	rm -f *.o 