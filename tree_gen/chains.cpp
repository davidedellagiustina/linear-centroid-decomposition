#include "../src/main.hpp"
#include <time.h>
using namespace std;

// Generate a tree make of a root and n/k chains with k nodes

int main(int argc, char* argv[]) { // Args: 1 => number of nodes, args[2] => number of nodes in each chain (1 <= k <= n)
	uint32_t n, k, n_k;
	if (argc < 2) return 1;
    else {
        n = atoi(argv[1]);
		k = atoi(argv[2]);
    }
	if (k < 1 || k > n) return 1;
    n_k = ((uint32_t) (n / k));
    oss os;
	os << "("; // Root of the tree
    string chain;
    // Build single chain
    oss c;
    for (uint32_t i = 0; i < k; ++i) c << "(";
    for (uint32_t i = 0; i < k; ++i) c << ")";
    chain = c.str();
    for (uint32_t i = 0; i < n_k; ++i) os << chain; // Concat n/k chains to root
    os << ")";
	cout << os.str();
	return 0;
}