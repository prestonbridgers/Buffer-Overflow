main: demo.h reader.c main.c 
	gcc reader.c main.c -lncurses -pthread -o run.out

run:
	./run.out 2> out.txt

clean:
	@rm *.out
