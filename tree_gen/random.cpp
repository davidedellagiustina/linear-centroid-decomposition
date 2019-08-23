#include "../src/main.hpp"
#include <time.h>
using namespace std;

// Global variables
uint32_t n, layer = 0;

// Main
int main(int argc, char* argv[]) {
	if (argc < 2) return 0;
	else {
		iss is(argv[1]);
		is >> n;
	}
    oss os; // Initialize output stream
    srand((unsigned int)time(0)); // Set seed for 'rand()'
    while (n > 0) { // While there are still nodes to add
        int p = rand() % 2; // Random number between 0 and 1
        if (p == 0 && layer > 1) { // If 0 and you are above layer 1
            os << ")"; // Close opened node
            layer--; // Decrement 'layer'
        } else { // Else
            os << "("; // Create new node
            layer++; n--; // Increment 'layer' and decrement 'n'
        }
    }
    for (; layer > 0; layer--) os << ")"; // Close each node remained open
    cout << os.str(); // Print content of output stream
    return 0;
}