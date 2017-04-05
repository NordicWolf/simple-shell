CC=gcc
CFLAGS=-c -Wall
LIBFLAGS=-lrt
OBJS=shell.o main.o

all: build exec

build:  $(OBJS)
	$(CC) lib/* -o bin/shell.bin $(LIBFLAGS)

main.o: src/main.c
	$(CC) $(CFLAGS) $^ -o lib/$@

shell.o: src/shell.c
	$(CC) $(CFLAGS) $^ -o lib/$@

clean:
	rm -vf bin/*

cleanall:
	rm -vf bin/* lib/*.o

exec:
	bin/shell.bin
