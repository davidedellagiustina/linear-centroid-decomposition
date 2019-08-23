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
void help(){
	cout << "Usage: nlognCD [options]" << nl <<
	"Options:" << nl <<
	" -h        Print this help." << nl <<
	" -i <arg>  Input tree [REQUIRED]." << nl <<
	" -o        Print output centroid tree." << nl <<
	" -c        Check correctness." << nl;
	exit(0);
}


// Main function
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
	cout << "Processing file " << input_path << "..." << nl;
    if (input_path.compare("") == 0) help(); // If no input is given
	ifstream in(input_path); in >> tree; in.close(); // Copy input into string
	chrono::high_resolution_clock::time_point t1 = getTime();
    try {
        // t = buildTree(tree);
        t = buildTree_l(tree); // Try building 't' (if not too big)
    } catch (const char* err) {
        cout << err << nl; // Or otherwise print exception
        return 0;
    }
    cout << printTime("T building", t1, getTime()) << nl;
    chrono::high_resolution_clock::time_point t2 = getTime();
    id_ref = buildIdRef(t); // Build the bitvector for nodes ID reference on 't'
	if (check) id_ref_copy = id_ref;
    cout << printTime("T bitvector building", t2, getTime()) << nl;
    chrono::high_resolution_clock::time_point t3 = getTime();
	// computeSizes(t, id_ref);
    computeSizes_l(t, id_ref); // Compute the initial sizes on 't'
    if (check) t_copy = t;
    cout << printTime("Computing sizes", t3, getTime()) << nl;
    cout << printTime("Total structure building", t1, getTime()) << nl; // Total time
    // Perform O(n*log(n)) time centroid decomposition of 't': O(n*log(n))
    t1 = getTime();
    ctree = stdCentroidDecomposition(t, id_ref, 0);
    cout << printTime("Standard centroid decomposition", t1, getTime()) << nl;
	if(check) cout << "Correctness check: " << ((checkCorrectness(t_copy, id_ref_copy, ctree))? "true" : "false") << nl; // Correctness check
    if (print_output) cout << "Output: " << ctree << nl; // Print output
    return 0;
}