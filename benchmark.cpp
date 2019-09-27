#include "src/main.hpp"
#include <cstring>
#include <stdlib.h>
using namespace std;

// Return the formatted string for the elapsed time
// @param duration  time in microseconds
// @return          formatted string
inline string printDuration(const uint64_t duration) {
    int us = duration; // Microseconds
    int ms = us/1000; us %= 1000; // Milliseconds
    int s = ms / 1000; ms %= 1000; // Seconds
    int m = s/60; s %= 60; // Minutes
    int h = m/60; m %= 60; // Hours
    oss os; os << h << "h " << m << "m " << s << "s " << ms << "ms " << us << "us"; // Format string
    return os.str();
}

// Perform standard centroid decomposition
// @param t         minimal representation of input tree
// @param check     perform correctness check?
// @return          execution time
inline uint32_t nlognCD(vector<uint32_t> t, const bool check) { // Complexity: O(n*log(n))
    chrono::high_resolution_clock::time_point t01 = getTime();
    uint32_t n = (t.size() + 2) / 4; // Number of nodes
    vector<uint32_t> id_ref = buildIdRef(t);
    computeSizes(t, id_ref);
    vector<uint32_t> t_cp;
    if (check) t_cp = t; // Copy tree for correctness check
    pair<vector<uint8_t>,vector<uint32_t>> ct = stdCentroidDecomposition(t);
    uint32_t time = chrono::duration_cast<chrono::microseconds>(getTime()-t01).count();
    if (check) cerr << "O(n*log(n)) - " << n << " nodes - correct: " << ((checkCorrectness(t_cp, ct))? "true" : "false") << nl;
    return time;
}

// Perform linear centroid decomposition
// @param t         minimal representation of input tree
// @param check     perform correctness check?
// @param A         size of subtrees [log(n) if not given]
// @param B         linear centroid decomposition threshold [log^3(n) if not given]
// @return          execution time
inline uint32_t nCD(vector<uint32_t> t, const bool check, const uint32_t A, const uint32_t B) { // Complexity: O(n)
    chrono::high_resolution_clock::time_point t01 = getTime();
    uint32_t n = (t.size() + 2) / 4; // Number of nodes
    vector<uint32_t> id_ref = buildIdRef(t);
    vector<uint32_t> t2 = cover(t, id_ref, A);
    vector<uint32_t> t_cp;
    if (check) t_cp = t; // Copy tree for correctness check
    pair<vector<uint8_t>,vector<uint32_t>> ct = centroidDecomposition(t, t2, B);
    uint32_t time = chrono::duration_cast<chrono::microseconds>(getTime()-t01).count();
    if (check) cerr << "O(n) - " << n << " nodes - correct: " << ((checkCorrectness(t_cp, ct))? "true" : "false") << nl;
    return time;
}

// Print help
void help() {
	cout << "Usage: benchmark [options]" << nl <<
	"Options:" << nl <<
	" -h        Print this help." << nl <<
    " -g <arg>  Tree generator. Options: random, path, binpath." << nl <<
    " -b <arg>  Number of nodes of smallest tree [REQUIRED]." << nl <<
    " -e <arg>  Number of nodes of biggest tree [REQUIRED]." << nl <<
    " -s <arg>  Increment step [REQUIRED]." << nl <<
    " -t <arg>  Number of tests for each tree [REQUIRED]." << nl <<
	" -c        Check correctness." << nl;
	exit(0);
}

// General options
string g = "random";
uint32_t start = 0, stop = 0, step = 0, tests = 0;
bool check = false; // Perform correctness check?
uint32_t A = 0;
uint32_t B = 0;

string tree;
vector<uint32_t> t;

int main(int argc, char* argv[]) {
    // Process command line options
	int opt;
	while ((opt = getopt(argc, argv, "hg:b:e:s:t:c")) != -1) {
		switch (opt) {
			case 'h':
				help();
				break;
            case 'g':
                if (strcmp("random", optarg) == 0 || strcmp("path", optarg) == 0) g = optarg;
                else if (strcmp("binpath", optarg) == 0) g = "binary_halfn";
                break;
			case 'b':
				start = atoi(optarg);
				break;
			case 'e':
				stop = atoi(optarg);
				break;
			case 's':
				step = atoi(optarg);
				break;
			case 't':
				tests = atoi(optarg);
				break;
			case 'c':
				check = true;
				break;
			default:
				help();
				return -1;
		}
	}
    if (!start || !stop || !step || !tests || stop < start) help(); // If incorrect parameters
    // Benchmark
    for (uint32_t n = start; n <= stop; n += step) {
        uint32_t t01 = 0, t02 = 0;
        for (uint32_t i = 0; i < tests; i++) { // Loop 'tests' times
            auto r = system(("tree_gen/" + g + " " + to_string(n) + " > tree.txt").c_str()); // Generate random tree
            ifstream in("tree.txt"); in >> tree; in.close();
            try {
                t = buildTree(tree); // Build tree (minimal representation)
            } catch (const char* err) {
                cout << err << nl;
                return 0;
            }
            t01 += nlognCD(t, check); // Perform O(n*log(n)) centroid decomposition
            t02 += nCD(t, check, A, B); // Perform O(n) centroid decomposition
        }
        t01 /= tests; t02 /= tests; // Compute average of 'tests' decompositions (both)
        cout << "O(n*log(n)) - " << n << " nodes - time: " << t01 << " -  formatted: " << printDuration(t01) << nl;
        cout << "O(n) - " << n << " nodes - time: " << t02 << " - formatted: " << printDuration(t02) << nl << nl;
        if (!check) cerr << "Done for " << n << " nodes." << nl;
        else cerr << nl;
    }
    auto r = system("rm -rf tree.txt"); // Delete 'tree.txt' temporary file
    return 0;
}