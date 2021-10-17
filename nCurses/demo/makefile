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

$(BIN): $(OBJS) | binarydir
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $(BINDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/demo.h | objectdir
	$(CC) $(CFLAGS) -c $< -o $@

objectdir:
	@mkdir -p $(OBJDIR)

binarydir:
	@mkdir -p $(BINDIR)

run:
	./$(BINDIR)/$(BIN) 2> err.log

clean:
	@rm $(OBJDIR)/* $(BINDIR)/* err.log output.txt
