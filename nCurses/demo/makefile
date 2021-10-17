CC=gcc
CFLAGS=-g -Wall
LFLAGS=-lncurses -pthread
OBJS=reader.o main.o
BIN=main

all: $(BIN)

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o run.out

%.o: %.c demo.h
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./run.out 2> out.txt

clean:
	@rm *.out *.o
