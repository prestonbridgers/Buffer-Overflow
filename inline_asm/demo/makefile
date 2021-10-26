SRCDIR=src
INCDIR=include
OBJDIR=obj
BINDIR=bin

CC=gcc
CFLAGS=-Wall -g -I$(INCDIR)
LFLAGS=-lpanel -lcurses

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

BIN=nlines_test

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) -o $(BINDIR)/$@ $(OBJS)

$(OBJDIR)/ret_addr.o: $(SRCDIR)/ret_addr.c $(INCDIR)/nlines.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/nlines_create.o: $(SRCDIR)/nlines_create.c $(INCDIR)/nlines.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/nlines_destroy.o: $(SRCDIR)/nlines_destroy.c $(INCDIR)/nlines.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/nlines_draw.o: $(SRCDIR)/nlines_draw.c $(INCDIR)/nlines.h
	$(CC) $(CFLAGS) -c -o $@ $<

run:
	@./$(BINDIR)/$(BIN)

memtest:
	@valgrind --leak-check=yes ./$(BINDIR)/$(BIN) 2> err.log

clean:
	rm $(OBJDIR)/* $(BINDIR)/*
