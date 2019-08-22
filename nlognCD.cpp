#include "src/main.hpp"
#include <unistd.h>

using namespace std;

/*
 * PERFORM CENTROID DECOMPOSITION IN LINEAR TIME
 */

// Settings
uint32_t A = (uint32_t)1e3;
uint32_t B = (uint32_t)1e3;

// Global variables
bool print_output = false, check=false;
string tree, ctree;
uint32_t n, _t_root;
vector<uint32_t> t, t_copy, _t;
vector<bool> id_ref, id_ref_copy, _id_ref;


void help(){

	cout << "nlognCD [options]" << endl <<
	"Options:" << endl <<
	"-h        Print this help." << endl <<
	"-i <arg>  Input tree [REQUIRED]." << endl <<
	"-o        Print output centroid tree." << endl <<
	"-c        Check correctness." << endl;
	exit(0);
}


// Main function. Complexity: O(n)
int main(int argc, char* argv[]) { // 'Argv[1]' must be the name of the file containing the tree structure (in BP representation), 'argv[2]' should be 'true' if you want to print output, argv[3] should be 'true' 

    string input_path;
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

    cout << "processing " << input_path<<endl;

    if(input_path.compare("")==0) help();
 
    ifstream in(input_path); in >> tree; in.close(); // Copy input into string

    chrono::high_resolution_clock::time_point t1 = getTime(); // Get initial time
    n = tree.length() / 2;
    try {
        t = buildTree_l(tree); // Try building 't' (if not too big)
    } catch (const char* err) {
        cout << err << nl; // Or otherwise print exception
        return 0;
    }
    cout << printTime("T building", t1, getTime()) << nl; // Print time to build T
    chrono::high_resolution_clock::time_point t2 = getTime();
    id_ref = buildIdRef(t); // Build the bitvector for nodes ID reference on 't'
    cout << printTime("Building bitvector", t2, getTime()) << nl;
    chrono::high_resolution_clock::time_point t3 = getTime();
    
    vector<bool> id_ref_copy;
    if(check) id_ref_copy = id_ref;//perform expensive copy only if needed!

    computeSizes(t, id_ref); // Compute the initial sizes on 't'
    
    vector<uint32_t> t_copy;
    if(check) t_copy = t;//perform expensive copy only if needed!

    cout << printTime("Computing sizes", t3, getTime()) << nl;
    
    cout << printTime("Structure building", t1, getTime()) << nl; // Print time to build structure
    // Perform nlogn-time centroid decomposition of 't': O(n)
    t1 = getTime(); // Get initial time
    ctree = stdCentroidDecomposition(t, id_ref, 0);
    cout << printTime("Standard centroid decomposition", t1, getTime()) << nl; // Print time to compute centroid decomposition
    if (print_output) cout << ctree << nl; // Print output
    // Check correctness
    if(check) cout << "Correctness check: " << ((checkCorrectness(t_copy, id_ref_copy, ctree))? "true" : "false") << nl; // Correctness check
    return 0;
}
