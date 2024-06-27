CC=gcc
CFLAGS=-Wall

.PHONY: all requester provider

all: requester provider

provider:
	$(CC) $(CFLAGS) provider.c -o provider

requester:
	$(CC) $(CFLAGS) requester.c -o requester

clean:
	rm -f requester provider pipe1 pipe2