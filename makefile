.DEFAULT_GOAL := install

CC = g++
CFLAGS = -g -ofast

install:
	$(CC) $(CFLAGS) cdlin.cpp -o cdlin
	$(CC) $(CFLAGS) cdstd.cpp -o cdstd

tools:
	$(CC) $(CFLAGS) tree_gen/random.cpp -o tree_generators/random
	$(CC) $(CFLAGS) tree_gen/path.cpp -o tree_generators/path
	$(CC) $(CFLAGS) tree_gen/binary_halfn.cpp -o tree_generators/binary_halfn
	$(CC) $(CFLAGS) benchmark.cpp -o benchmark

all: tools install

clear:
	rm -rf cdlin
	rm -rf cdstd
	rm -rf tree_gen/random
	rm -rf tree_gen/path
	rm -rf tree_gen/binary_halfn
	rm -rf benchmark