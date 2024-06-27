CXX=gcc
CFLAGS=-Wall -O0

.PHONY: all static shared dynamic clean


all: static shared dynamic

#static library
static: static.o collatz_static.a
	$(CXX) $(CFLAGS) -o static static.o collatz_static.a

static.o:
	$(CXX) $(CFLAGS) -c main.c -o static.o

collatz_static.a: collatz_static.o
	ar rcs collatz_static.a collatz_static.o

collatz_static.o:
	$(CXX) $(CFLAGS) -c collatz.c -o collatz_static.o

#shared library
shared: libcollatz.so
	$(CXX) $(CFLAGS) main.c -lcollatz -L./ -Wl,-rpath,. -o shared

collatz_shared.o:
	$(CXX) $(CFLAGS) -c -fPIC collatz.c -o collatz_shared.o

libcollatz.so: collatz_shared.o
	$(CXX) -shared -o libcollatz.so collatz_shared.o

#dynamic library
dynamic: dynamic.o libcollatz.so
	$(CXX) $(CFLAGS) dynamic.o -Wl,-rpath,. -o dynamic

dynamic.o:
	$(CXX) $(CFLAGS) -DUSE_DYNAMIC -c main.c -o dynamic.o

clean:
	rm -f static static.o collatz_static.a collatz_static.o shared collatz_shared.o libcollatz.so dynamic.o dynamic

