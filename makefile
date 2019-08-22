.DEFAULT_GOAL := install

CC = g++
CFLAGS = -g -ofast

install:
	$(CC) $(CFLAGS) nCD.cpp -o nCD
	$(CC) $(CFLAGS) nlognCD.cpp -o nlognCD

tools:
	$(CC) $(CFLAGS) tree_generators/random.cpp -o tree_generators/random
	$(CC) $(CFLAGS) tree_generators/path.cpp -o tree_generators/path
	$(CC) $(CFLAGS) tree_generators/binary_height_half_n.cpp -o tree_generators/binary_height_half_n
	$(CC) $(CFLAGS) benchmark.cpp -o benchmark

all: tools install

clear:
	rm -rf nCD
	rm -rf nlognCD
	rm -rf tree_generators/random
	rm -rf tree_generators/path
