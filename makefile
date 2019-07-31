.DEFAULT_GOAL := install

CC = g++
CFLAGS = -g

install:
	$(CC) $(CFLAGS) main.cpp -o main

clean:
	rm -rf main
	rm -rf randomTree
	rm -rf benchmark

tools:
	$(CC) $(CFLAGS) randomTree.cpp -o randomTree
	$(CC) $(CFLAGS) benchmark.cpp -o benchmark

all: tools install