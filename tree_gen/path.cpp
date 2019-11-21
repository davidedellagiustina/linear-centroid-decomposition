#include "../src/main.hpp"
#include <time.h>
using namespace std;

// Generate path of <n> nodes, each one with degree <k>

int main(int argc, char* argv[]) { // Args: 1 => number of nodes, args[2] => degree of the tree (1 <= k <= n)
	int64_t n;
	uint32_t k;
	if (argc < 2) return 1;
    else {
        n = atoi(argv[1]);
		k = atoi(argv[2]);
    }
	if (k < 1 || k > n) return 1;
    oss b, e;
	b << "("; e << ")"; --n; // Root of the tree
	srand((unsigned int)time(0)); // Set random seed
	while (n > 0) {
		int c = 1 + (rand() % k);
		for (int i = 0; i < c-1; ++i) b << "()";
		for (int i = 0; i < k-c; ++i) e << ")(";
		b << "("; e << ")";
		n -= k;
	}
	string end = e.str();
    reverse(end.begin(), end.end());
	cout << b.str() << end;
	return 0;
}