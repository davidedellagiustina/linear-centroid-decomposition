.DEFAULT_GOAL := install

CC = g++
CFLAGS = -g -ofast

install:
	$(CC) $(CFLAGS) main.cpp -o main

tools:
	$(CC) $(CFLAGS) tree_generators/random.cpp -o tree_generators/random
	$(CC) $(CFLAGS) tree_generators/binary_height_half_n.cpp -o tree_generators/binary_height_half_n
	$(CC) $(CFLAGS) benchmark.cpp -o benchmark

all: tools install

clear:
	rm -rf main
	rm -rf tree_generators/random