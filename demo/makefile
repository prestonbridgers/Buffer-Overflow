CC=gcc
CFLAGS=-g -Wall
LFLAGS=-pthread -lpanel -lcurses

bovis: layout.o
	$(CC) $(LFLAGS) $^ -o $@

layout.o: layout.c 
	$(CC) $(CFLAGS) -c $< -o $@

run:
	@./bovis 2> err.log

clean:
	rm bovis
	rm *.o
	rm *.log
	rm prog.out
