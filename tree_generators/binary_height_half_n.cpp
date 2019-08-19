#include <iostream>
using namespace std;

// Generate binary tree with height n/2
int main() {
	uint32_t n = (uint32_t)10e6; // Number of nodes
	for (uint32_t i = 0; i < n/2; i++) cout << "(()";
	cout << "()";
	for(uint32_t i = 0; i < n/2; i++) cout << ")";
	return 0;
}