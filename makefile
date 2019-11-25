.DEFAULT_GOAL := install

CC = g++
CFLAGS = -g -O3 -mtune=native -march=native

std:
	$(CC) $(CFLAGS) cdstd.cpp -o cdstd

lin:
	$(CC) $(CFLAGS) cdlin.cpp -o cdlin

install: std lin

tools:
	$(CC) $(CFLAGS) tree_gen/random.cpp -o tree_gen/random
	$(CC) $(CFLAGS) tree_gen/path.cpp -o tree_gen/path
	$(CC) $(CFLAGS) tree_gen/chains.cpp -o tree_gen/chains
	$(CC) $(CFLAGS) tree_gen/binary_halfn.cpp -o tree_gen/binary_halfn
	$(CC) $(CFLAGS) benchmark.cpp -o benchmark

all: tools install

clean:
	rm -rf cdlin
	rm -rf cdstd
	rm -rf tree_gen/random
	rm -rf tree_gen/path
	rm -rf tree_gen/chains
	rm -rf tree_gen/binary_halfn
	rm -rf benchmark