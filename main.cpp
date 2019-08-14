#include "src/main.hpp"
using namespace std;

/*
 * PERFORM CENTROID DECOMPOSITION IN LINEAR TIME
 */

// Global variables
string tree;
uint32_t _t_root;
vector<uint32_t> t, _t;
vector<bool> id_ref, _id_ref;

// Main function. Complexity: O(n)
int main(int argc, char* argv[]) { // 'Argv[1]' must be the name of the file containing the tree structure (in BP representation)
    if (argc < 2) {
        cout << "No input file specified." << endl;
        return 0;
    }
    // Build 't' representation: O(n)
    ifstream in(argv[1]); in >> tree; in.close(); // Copy input into string
    chrono::high_resolution_clock::time_point t1 = getTime(); // Get initial time
    try {
        t = buildTree(tree); // Try building 't' (if not too big)
    } catch (const char* err) {
        cout << err << endl; // Or otherwise print exception
        return 0;
    }
    id_ref = buildIdRef(t); // Build the bitvector for nodes ID reference on 't'
    computeSizes(t, id_ref); // Compute the initial sizes on 't'
    pair<uint32_t,vector<uint32_t>> tmp = cover(t, id_ref); // Cover 't'
    _t_root = tmp.first; _t = tmp.second;
    _id_ref = build_IdRef(_t); // Build the bitvector for nodes ID reference on '_t'
    cout << printTime("Structure building", t1, getTime()) << endl; // Print time to build structure
    // Perform linear-time centroid decomposition of 't': O(n)
    t1 = getTime(); // Get initial time
    cout << centroidDecomposition(t, id_ref, _t_root, _t, _id_ref); // Perform linear-time centroid decomposition
    cout << printTime("Linear centroid decomposition", t1, getTime()) << endl; // Print time to compute centroid decomposition
    return 0;
}