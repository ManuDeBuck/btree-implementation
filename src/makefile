CC=gcc
CFLAGS=-I.

all: history

history: main.c btree.c util.c btree.h util.h
	$(CC) -O3 -o history main.c btree.c btree.h util.c

clean:
	rm -f history