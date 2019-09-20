#include "../src/main.hpp"
#include <time.h>
using namespace std;

// Generate completely random tree

int main(int argc, char* argv[]) { // Args: 1 => number of nodes
    uint32_t n;
    if (argc < 2) return 1;
    else {
        iss is(argv[1]);
        is >> n; // Get number of nodes
    }
    oss os;
    srand((unsigned int)time(0)); // Set random seed
    uint32_t layer = 0;
    while (n > 0) {
        int c = rand() % 2; // Random: 0 => close node, 1 => new node
        if (c == 0 && layer > 1) { // Layer check before closing node
            os << ")";
            layer--;
        } else {
            os << "(";
            layer++; n--;
        }
    }
    for (; layer > 0; layer--) os << ")"; // Close each "unclosed" node
    cout << os.str();
    return 0;
}