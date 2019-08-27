#include "src/main.hpp"

using namespace std;

/*
 * PERFORM CENTROID DECOMPOSITION IN O(n*log(n)) TIME
 */

// Global variables
bool print_output = false, check = false;
string input_path, tree, ctree;
uint32_t _t_root;
vector<uint32_t> t, t_copy, _t;
vector<bool> id_ref, id_ref_copy, _id_ref;

// Print help
void help() {
	cout << "Usage: cdstd [options]" << nl <<
	"Options:" << nl <<
	" -h        Print this help." << nl <<
	" -i <arg>  Input tree [REQUIRED]." << nl <<
	" -o        Print output centroid tree." << nl <<
	" -c        Check correctness." << nl;
	exit(0);
}

// Main
int main(int argc, char* argv[]) { // Complexity: O(n*log(n))
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
	if (check) id_ref_copy = id_ref;
    cout << printTime(" - T reference bitvector building", t2, getTime()) << nl;
	// Compute partial sizes on 't'
    chrono::high_resolution_clock::time_point t3 = getTime();
    computeSizes(t, id_ref);
    if (check) t_copy = t;
    cout << printTime(" - Computing sizes", t3, getTime()) << nl;
    cout << printTime(" - Total structure building", t1, getTime()) << nl; // Total time
    // Perform centroid decomposition: O(n*log(n))
    t1 = getTime();
    ctree = stdCentroidDecomposition(t, id_ref, 0);
    cout << printTime(" - Standard centroid decomposition", t1, getTime()) << nl;
	if(check) cout << "Correct: " << ((checkCorrectness(t_copy, id_ref_copy, ctree))? "true" : "false") << nl; // Correctness check
    if (print_output) cout << "Output: " << ctree << nl; // Print output
    return 0;
}