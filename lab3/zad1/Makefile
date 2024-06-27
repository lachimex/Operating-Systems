CC=gcc
CFLAGS=-Wall

.PHONY: all reversed reversed10

all: reversed1 reversed10

reversed1: one_byte.o
	$(CC) $(CFLAGS) one_byte.o -o reversed1

one_byte.o:
	$(CC) $(CFLAGS) -c byte.c -o one_byte.o

reversed10: more_bytes.o
	$(CC) $(CFLAGS) more_bytes.o -o reversed10

more_bytes.o:
	$(CC) $(CFLAGS) -c bytes.c -o more_bytes.o

clean:
	rm -f reversed1 reversed10 *.o copied*.txt

