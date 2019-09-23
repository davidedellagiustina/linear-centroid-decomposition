#include "src/main.hpp"
using namespace std;

/*
 * PERFORM CENTROID DECOMPOSITION IN O(n) TIME
 */

// Global
bool print_output = false, check = false;
string input_path, tree;
uint32_t n, A = 0, B = 0;
vector<uint32_t> t, t_cp, id_ref, t2, ct;

// Print help
void help() {
	cout << "Usage: cdlin [options]" << nl <<
	"Options:" << nl <<
	" -h        Print this help." << nl <<
	" -i <arg>  Input tree [REQUIRED]." << nl <<
	" -A <arg>	Size of trelets for tree covering." << nl <<
	" -B <arg>	Threshold for linear centroid decomposition." << nl <<
	" -o        Print output centroid tree." << nl <<
	" -c        Check correctness." << nl;
	exit(0);
}

int main(int argc, char* argv[]) {
	// Process command line options
	int opt;
	while ((opt = getopt(argc, argv, "hoci:A:B:")) != -1) {
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
			case 'A':
				A = atoi(optarg);
				break;
			case 'B':
				B = atoi(optarg);
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
	// Tree covering
	chrono::high_resolution_clock::time_point t02 = getTime();
	try {
		t2 = cover(t, id_ref, A);
	} catch (const char* err) {
		cout << err << nl;
		return -1;
	}
	cout << printTime(" - Tree covering and partial sizes", t02, getTime()) << nl;
	cout << printTime(" - Total structure building", t01, getTime()) << nl; // Total time
	// Copy structures
	if (check) t_cp = t;
	// Perform centroid decomposition: O(n)
	t01 = getTime();
	ct = centroidDecomposition(t, t2, B);
	cout << printTime(" - Linear centroid decomposition", t01, getTime()) << nl;
	if(check) cout << "Correct: " << ((checkCorrectness(t_cp, ct))? "true" : "false") << nl; // Correctness check
	if (print_output) cout << "Output: " << ctToString(ct) << nl; // Print output
	return 0;
}