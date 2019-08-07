#include "src/main.hpp"
using namespace std;

int main(int argc, char* argv[]) { // 'Argv[1]' must be the name of the file containing the tree structure (in BP representation)
    if (argc < 2) {
        cout << "No input file specified." << endl;
        return 0;
    }
    // Build 't'
    string tree;
    ifstream in(argv[1]); in >> tree; in.close(); // Copy input into string
    vector<uint32_t> t;
    vector<bool> id_ref;
    chrono::high_resolution_clock::time_point t1 = getTime(); // Get initial time
    try {
        t = buildTree(tree); // Try building 't' (if not too big)
    } catch (const char* err) {
        cout << err << endl; // Or otherwise print exception
        return 0;
    }
    id_ref = buildIdRef(t); // Build the bitvector for nodes ID reference
    computeSizes(t, id_ref); // Compute the initial sizes on 't'
    vector<uint32_t> _t = cover(t, id_ref); // Cover 't'
    cout << printTime("Structure building", t1, getTime()) << endl; // Print time to build structure

    // TEMP - standard centroid decomposition in n*log(n)
    // t1 = getTime(); // Get new time
    // stdCentroidDecomposition(t); // Perform centroid decomposition
    // cout << printTime("Centroid decomposition in n*log(n)", t1, getTime()) << endl; // Print time to compute centroid decomposition
    return 0;
}