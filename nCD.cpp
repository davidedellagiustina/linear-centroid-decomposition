#include "src/main.hpp"
using namespace std;

/*
 * PERFORM CENTROID DECOMPOSITION IN LINEAR TIME
 */

// Settings
uint32_t A = (uint32_t)1e3;
uint32_t B = (uint32_t)1e3;

// Global variables
bool print_output = false, check = false;
string input_path, tree, ctree;
uint32_t n, _t_root;
vector<uint32_t> t, t_copy, _t;
vector<bool> id_ref, id_ref_copy, _id_ref;

// Print help
void help(){
	cout << "Usage: nCD [options]" << nl <<
	"Options:" << nl <<
	" -h        Print this help." << nl <<
	" -i <arg>  Input tree [REQUIRED]." << nl <<
	" -o        Print output centroid tree." << nl <<
	" -c        Check correctness." << nl;
	exit(0);
}

// Main function
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
	cout << "Processing file " << input_path << "..." << nl;
	if (input_path.compare("") == 0) help(); // If no input is given
    ifstream in(input_path); in >> tree; in.close(); // Copy input into string
    chrono::high_resolution_clock::time_point t1 = getTime();
    n = tree.length() / 2;
    try {
        t = buildTree(tree); // Try building 't' (if not too big)
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
    computeSizes(t, id_ref); // Compute the initial sizes on 't'
    if (check) t_copy = t;
    cout << printTime("Computing sizes", t3, getTime()) << nl;
	chrono::high_resolution_clock::time_point t4 = getTime();
    pair<uint32_t,vector<uint32_t>> tmp = cover(t, id_ref, A); // Cover 't'
    _t_root = tmp.first; _t = tmp.second;
	cout << printTime("Tree covering", t4, getTime()) << nl;
	chrono::high_resolution_clock::time_point t5 = getTime();
    _id_ref = build_IdRef(_t, n); // Build the bitvector for nodes ID reference on '_t'
	cout << printTime("T'' bitvector building", t5, getTime()) << nl;
	cout << printTime("Total structure building", t1, getTime()) << nl; // Total time
    // Perform linear-time centroid decomposition of 't': O(n)
    t1 = getTime();
    ctree = centroidDecomposition(t, id_ref, _t_root, _t, _id_ref, B);
    cout << printTime("Linear centroid decomposition", t1, getTime()) << nl;
    if(check) cout << "Correctness check: " << ((checkCorrectness(t_copy, id_ref_copy, ctree))? "true" : "false") << nl; // Correctness check
    if (print_output) cout << "Output: " << ctree << nl; // Print output
    return 0;
}
