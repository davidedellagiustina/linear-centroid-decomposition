#include <iostream>
using namespace std;

// Generate path of <arg> nodes
int main(int argc, char* argv[]) {
	uint32_t n = atoi(argv[1]); // Number of nodes
	for (uint32_t i = 0; i < n; i++) cout << "(";
	for (uint32_t i = 0; i < n; i++) cout << ")";
	return 0;
}
