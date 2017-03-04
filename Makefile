CC=gcc 
CFLAGS=-lrt -pthread

all: assn2
program: assn2.o


clean:
	rm -f assn2 assn2.o
run:
	./assn2
