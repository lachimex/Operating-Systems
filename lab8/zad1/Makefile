CC=gcc
CFLAGS=-Wall

.PHONY: all printers users

all: printers users

printers:
	$(CC) $(CFLAGS) printers.c -lrt -o printers

users:
	$(CC) $(CFLAGS) users.c -lrt -o users

clean:
	rm -f printers users