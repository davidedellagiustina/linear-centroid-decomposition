#include "src/main.hpp"
using namespace std;

/*
 * PERFORM CENTROID DECOMPOSITION IN LINEAR TIME
 */

// Settings
uint32_t A = (uint32_t)1e3;
uint32_t B = (uint32_t)1e3;

// Global variables
bool print_output = false;
string tree, ctree;
uint32_t n, _t_root;
vector<uint32_t> t, t_copy, _t;
vector<bool> id_ref, id_ref_copy, _id_ref;

// Main function. Complexity: O(n)
int main(int argc, char* argv[]) { // 'Argv[1]' must be the name of the file containing the tree structure (in BP representation), 'argv[2]' should be 'true' if you want to print output
    if (argc < 2) {
        cout << "No input file specified." << endl;
        return 0;
    }
    if (argc >= 3 && string(argv[2]) == "true") print_output = true; // If 'argv[2]' == 'true' then output will be printed
    // Build 't' representation: O(n)
    ifstream in(argv[1]); in >> tree; in.close(); // Copy input into string
    chrono::high_resolution_clock::time_point t1 = getTime(); // Get initial time
    n = tree.length() / 2;
    try {
        t = buildTree(tree); // Try building 't' (if not too big)
    } catch (const char* err) {
        cout << err << nl; // Or otherwise print exception
        return 0;
    }
    id_ref = buildIdRef(t); // Build the bitvector for nodes ID reference on 't'
    id_ref_copy = id_ref;
    computeSizes(t, id_ref); // Compute the initial sizes on 't'
    t_copy = t;
    pair<uint32_t,vector<uint32_t>> tmp = cover(t, id_ref, A); // Cover 't'
    _t_root = tmp.first; _t = tmp.second;
    _id_ref = build_IdRef(_t, n); // Build the bitvector for nodes ID reference on '_t'
    cout << printTime("Structure building", t1, getTime()) << nl; // Print time to build structure
    // Perform linear-time centroid decomposition of 't': O(n)
    t1 = getTime(); // Get initial time
    ctree = centroidDecomposition(t, id_ref, _t_root, _t, _id_ref, B); // Perform linear-time centroid decomposition
    cout << printTime("Linear centroid decomposition", t1, getTime()) << nl; // Print time to compute centroid decomposition
    if (print_output) cout << ctree << nl; // Print output
    // Check correctness
    cout << "Correctness check: " << ((checkCorrectness(t_copy, id_ref_copy, ctree))? "true" : "false") << nl; // Correctness check
    return 0;
}