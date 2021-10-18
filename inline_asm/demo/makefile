default:
	gcc -g -Wall ret_addr.c -o run.out

run:
	@aslr.sh disable > /dev/null
	@./run.out
	@aslr.sh enable > /dev/null
