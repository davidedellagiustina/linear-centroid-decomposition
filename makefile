.DEFAULT_GOAL := install

CC = g++
CFLAGS = -g

install:
	$(CC) $(CFLAGS) main.cpp -o main

tools:
	$(CC) $(CFLAGS) randomTree.cpp -o randomTree

all: tools install

clear:
	rm -rf main
	rm -rf randomTree