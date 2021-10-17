CC=gcc
CFLAGS=-g -Wall
LFLAGS=-lncurses -pthread

SRCDIR=src
BINDIR=bin
INCDIR=include
OBJDIR=obj

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

BIN=run.out

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $(BINDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/demo.h
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(BINDIR)/run.out 2> err.log

clean:
	@rm $(OBJDIR)/* $(BINDIR)/*
