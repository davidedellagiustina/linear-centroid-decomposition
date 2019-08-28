#include "src/main.hpp"
using namespace std;

/*
 * PERFORM CENTROID DECOMPOSITION IN LINEAR TIME
 */

// Settings
uint32_t A = 0;
uint32_t B = 0;

// Global variables
bool print_output = false, check = false;
string input_path, tree, ctree;
uint32_t n, _t_root;
vector<uint32_t> t, t_copy, _t;
vector<bool> id_ref;

// Print help
void help() {
	cout << "Usage: cdlin [options]" << nl <<
	"Options:" << nl <<
	" -h        Print this help." << nl <<
	" -i <arg>  Input tree [REQUIRED]." << nl <<
	" -o        Print output centroid tree." << nl <<
	" -c        Check correctness." << nl;
	exit(0);
}

// Main
int main(int argc, char* argv[]) { // Complexity: O(n)
    // Process command line options
	int opt;
	while ((opt = getopt(argc, argv, "hoci:")) != -1){
		switch (opt){
			case 'h':
				help();
				break;
			case 'o':
				print_output = true;
				break;
			case 'c':
				check = true;
				break;
			case 'i':
				input_path = string(optarg);
				break;
			default:
				help();
				return -1;
		}
	}
    // Centroid decomposition
	if (input_path.compare("") == 0) { cout << "Error: no input file." << nl << nl; help(); } // If no input is given
	cout << "Processing file '" << input_path << "'..." << nl << "Partial times:" << nl;
    ifstream in(input_path); in >> tree; in.close();
	// Build 't'
    chrono::high_resolution_clock::time_point t1 = getTime();
    n = tree.length() / 2;
    try {
        t = buildTree(tree);
    } catch (const char* err) {
        cout << err << nl;
        return 0;
    }
	cout << printTime(" - T building", t1, getTime()) << nl;
	// Build 't' reference bitvector
	chrono::high_resolution_clock::time_point t2 = getTime();
	id_ref = buildIdRef(t);
	cout << printTime(" - T reference bitvector building", t2, getTime()) << nl;
	// Tree covering
	chrono::high_resolution_clock::time_point t3 = getTime();
	pair<uint32_t,vector<uint32_t>> tmp = cover(t, id_ref, A);
	_t_root = tmp.first; _t = tmp.second;
	cout << printTime(" - Tree covering", t3, getTime()) << nl;
	// Compute partial sizes on't'
    chrono::high_resolution_clock::time_point t4 = getTime();
    computeSizes(t, id_ref);
    cout << printTime(" - Computing sizes", t4, getTime()) << nl;
	cout << printTime(" - Total structure building", t1, getTime()) << nl; // Total time
	// Copy structures
	if (check) t_copy = t;
    // Perform centroid decomposition: O(n)
    t1 = getTime();
    ctree = centroidDecomposition(t, _t_root, _t, B);
    cout << printTime(" - Linear centroid decomposition", t1, getTime()) << nl;
    if(check) cout << "Correct: " << ((checkCorrectness(t_copy, ctree))? "true" : "false") << nl; // Correctness check
    if (print_output) cout << "Output: " << ctree << nl; // Print output
    return 0;
}