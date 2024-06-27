CC=gcc
CFLAGS=-Wall

.PHONY: all

all: catcher sender

cather:
	$(CC) $(CFLAGS) catcher.c -o catcher

sender:
	$(CC) $(CFLAGS) sender.c -o sender

clean:
	rm -f catcher sender

