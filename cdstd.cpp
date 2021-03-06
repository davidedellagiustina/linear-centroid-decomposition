#include "src/main.hpp"
using namespace std;

/*
 * PERFORM CENTROID DECOMPOSITION IN O(n*log(n)) TIME
 */

// Global
bool print_output = false, check = false;
string input_path, tree;
vector<uint32_t> t, t_cp, id_ref;
struct c_tree ct;

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

int main(int argc, char* argv[]) {
	// Process command line options
	int opt;
	while ((opt = getopt(argc, argv, "hoci:")) != -1) {
		switch (opt) {
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
	cout << "Processing file '" << input_path << "'..." << nl;
	ifstream in(input_path); in >> tree; in.close();
	// Build T
	cout << "Building internal representation ..." << nl;
	try {
		t = buildTree(tree);
	} catch (const char* err) {
		cout << err << nl;
		return -1;
	}
	cout << "Partial times:" << nl;
	// Build T reference bitvector
	chrono::high_resolution_clock::time_point t01 = getTime();
	id_ref = buildIdRef(t);
	cout << printTime(" - T reference bitvector building", t01, getTime()) << nl;
	// Compute partial sizes on T
	chrono::high_resolution_clock::time_point t02 = getTime();
	computeSizes(t, id_ref);
	cout << printTime(" - Computing partial sizes", t02, getTime()) << nl;
	cout << printTime(" - Total structure building", t01, getTime()) << nl; // Total time
	// Copy structures
	if (check) t_cp = t;
	// Perform centroid decomposition: O(n*log(n))
	t01 = getTime();
	ct = stdCentroidDecomposition(t);
	cout << printTime(" - Standard centroid decomposition", t01, getTime()) << nl;
	if(check) cout << "Correct: " << ((checkCorrectness(t_cp, ct))? "true" : "false") << nl; // Correctness check
	if (print_output) cout << "Output: " << ctToString(ct) << nl; // Print output
	return 0;
}