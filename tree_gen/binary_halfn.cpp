#include "../src/main.hpp"
using namespace std;

// Generate binary tree with height n/2

int main(int argc, char* argv[]) { // Args: 1 => number of nodes
	uint32_t n;
	if (argc < 2) return 1;
	else {
		iss is(argv[1]);
		is >> n; // Get number of nodes
	}
	for (uint32_t i = 0; i < n/2; i++) cout << "(()";
	cout << "()";
	for(uint32_t i = 0; i < n/2; i++) cout << ")";
	return 0;
}