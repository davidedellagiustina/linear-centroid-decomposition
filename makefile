.DEFAULT_GOAL := install

CC = g++
CFLAGS = -g -O3 -mtune=native -march=native

install:
	$(CC) $(CFLAGS) cdlin.cpp -o cdlin
	$(CC) $(CFLAGS) cdstd.cpp -o cdstd

tools:
	$(CC) $(CFLAGS) tree_gen/random.cpp -o tree_gen/random
	$(CC) $(CFLAGS) tree_gen/path.cpp -o tree_gen/path
	$(CC) $(CFLAGS) tree_gen/binary_halfn.cpp -o tree_gen/binary_halfn
	$(CC) $(CFLAGS) benchmark.cpp -o benchmark

all: tools install

clean:
	rm -rf cdlin
	rm -rf cdstd
	rm -rf tree_gen/random
	rm -rf tree_gen/path
	rm -rf tree_gen/binary_halfn
	rm -rf benchmark
