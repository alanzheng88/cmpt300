CC=gcc 
CFLAGS=-lrt

all: assn2
program: assn2.o
assn2.o: assn2.c 

clean:
	rm -f assn2 assn2.o
run:
	./assn2
