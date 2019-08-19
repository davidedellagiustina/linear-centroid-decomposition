#include "src/main.hpp"
#include <cstring>
#include <stdlib.h>
using namespace std;

/*
 * BENCHMARK TOOL
 */

// Return the formatted string for the elapsed time (input in microseconds)
inline string printDuration(const uint64_t duration) {
    int us = duration; // Microseconds
    int ms = us/1000; us %= 1000; // Milliseconds
    int s = ms / 1000; ms %= 1000; // Seconds
    int m = s/60; s %= 60; // Minutes
    int h = m/60; m %= 60; // Hours
    oss os; // New output stream
    os << h << "h " << m << "m " << s << "s " << ms << "ms " << us << "us"; // Print to 'os'
    return os.str(); // Return stream content as a string
}

// Settings
uint32_t start = (uint32_t)1e6; // Starting point of benchmark (size)
uint32_t stop = (uint32_t)50e6; // Ending point of benchmark (size)
uint32_t step = (uint32_t)1e6; // Step at every cycle
bool check = true; // Check if every centroid decomposition was correct (very time-consuming!)

// Global variables
string tree, ctree_1, ctree_2;
uint32_t n, _t_root;
vector<uint32_t> t_1, t_2, t_copy_1, t_copy_2, _t;
vector<bool> id_ref_1, id_ref_2, id_ref_copy_1, id_ref_copy_2, _id_ref;

// Main function. Complexity: O(n)
int main() {
    for (uint32_t n = start; n <= stop; n += step) { // For every 'n'
        uint32_t time_1 = 0, time_2 = 0; // Initialize time
        for (uint32_t i = 0; i < 5; i++) { // Loop 5 times
            // Generate random tree
            system(("treeGen.exe " + to_string(n) + " > tree.txt").c_str()); // Generate random tree
            ifstream in("tree.txt"); in >> tree; in.close(); // Load generated tree
            // Perform O(n*log(n)) centroid decomposition
            chrono::high_resolution_clock::time_point t1 = getTime(); // Record initial time 1
            try {
                t_1 = buildTree(tree); // Build tree
            } catch (const char* err) {
                cout << err << nl;
                return 0;
            }
            id_ref_1 = buildIdRef(t_1); // Build 'id_ref'
            id_ref_copy_1 = id_ref_1;
            computeSizes(t_1, id_ref_1); // Compute partial sizes of the tree
            t_copy_1 = t_1;
            ctree_1 = stdCentroidDecomposition(t_1, id_ref_1, 0); // Compute the centroid decomposition in O(n*log(n))
            time_1 += chrono::duration_cast<chrono::microseconds>(getTime()-t1).count(); // Get duration 1
            if (check) cerr << "O(n*log(n)) - " << n << " nodes - correct: " << ((checkCorrectness(t_copy_1, id_ref_copy_1, ctree_1))? "true" : "false") << nl; // Check correctness
            // Perform O(n) centroid decomposition
            t1 = getTime(); // Record initial time 2
            try {
                t_2 = buildTree(tree); // Build tree
            } catch (const char* err) {
                cout << err << nl;
                return 0;
            }
            id_ref_2 = buildIdRef(t_2); // Build 'id_ref'
            id_ref_copy_2 = id_ref_2;
            computeSizes(t_2, id_ref_2); // Compute partial sizes of the tree
            t_copy_2 = t_2;
            pair<uint32_t,vector<uint32_t>> tmp = cover(t_2, id_ref_2); // Cover tree
            _t_root = tmp.first; _t = tmp.second;
            _id_ref = build_IdRef(_t, n); // Build '_id_ref'
            ctree_2 = centroidDecomposition(t_2, id_ref_2, _t_root, _t, _id_ref); // Compute the centroid decomposition in O(n)
            time_2 += chrono::duration_cast<chrono::microseconds>(getTime()-t1).count(); // Get duration 2
            if (check) cerr << "O(n) - " << n << " nodes - correct: " << ((checkCorrectness(t_copy_2, id_ref_copy_2, ctree_2))? "true" : "false") << nl; // Check correctness
        }
        time_1 /= 5; time_2 /= 5; // Compute average of 5 times (1 and 2)
        cout << "O(n*log(n)) - " << n << " nodes - time: " << time_1 << " -  formatted: " << printDuration(time_1) << nl; // Output average duration for O(n*log(n)) algorithm
        cout << "O(n) - " << n << " nodes - time: " << time_2 << " - formatted: " << printDuration(time_2) << nl << nl; // Output average duration for O(n) algorithm
        if (check) cerr << nl;
    }
    return 0; // Exit
}