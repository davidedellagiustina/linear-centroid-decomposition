#include "../src/main.hpp"
using namespace std;

// Global variables
uint32_t n;

// Generate binary tree with height n/2
int main(int argc, char* argv[]) {
	if (argc < 2) return 0;
	else {
		iss is(argv[1]);
		is >> n;
	}
	for (uint32_t i = 0; i < n/2; i++) cout << "(()";
	cout << "()";
	for(uint32_t i = 0; i < n/2; i++) cout << ")";
	return 0;
}