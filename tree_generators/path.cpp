#include <iostream>

using namespace std;

// Generate path of <arg> nodes. 
int main(int argc, char* argv[]) {
	uint32_t n = atoi(argv[1])*2; // Number of parentheses
	for (uint32_t i = 0; i < n/2; i++) cout << "(";
	for(uint32_t i = 0; i < n/2; i++) cout << ")";
	return 0;
}
