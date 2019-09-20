#include <iostream>
using namespace std;

// Generate path of <arg> nodes

int main(int argc, char* argv[]) { // Args: 1 => number of nodes
	uint32_t n = atoi(argv[1]);
	for (uint32_t i = 0; i < n; i++) cout << "(";
	for (uint32_t i = 0; i < n; i++) cout << ")";
	return 0;
}