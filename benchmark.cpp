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

// Perform standard centroid decomposition
inline uint32_t nlognCD(vector<uint32_t> &t, const bool &check) { // Complexity: O(n*log(n))
    chrono::high_resolution_clock::time_point t1 = getTime();
    uint32_t n = (t.size() + 2) / 4;
    vector<uint32_t> t_copy, ct;
    vector<bool> id_ref;
    id_ref = buildIdRef(t);
    computeSizes(t, id_ref);
    if (check) t_copy = t;
    ct = stdCentroidDecomposition(t);
    uint32_t time = chrono::duration_cast<chrono::microseconds>(getTime()-t1).count();
    if (check) cerr << "O(n*log(n)) - " << n << " nodes - correct: " << ((checkCorrectness(t_copy, ct))? "true" : "false") << nl;
    return time;
}

// Perform linear centroid decomposition
inline uint32_t nCD(vector<uint32_t> &t, const bool &check, const uint32_t &A, const uint32_t &B) { // Complexity: O(n)
    chrono::high_resolution_clock::time_point t1 = getTime();
    uint32_t n = (t.size() + 2) / 4;
    vector<uint32_t> t_copy, _t, ct;
    vector<bool> id_ref;
    uint32_t _t_root;
    id_ref = buildIdRef(t);
    computeSizes(t, id_ref);
    if (check) t_copy = t;
    pair<uint32_t,vector<uint32_t>> tmp = cover_old(t, A);
    _t_root = tmp.first; _t = tmp.second;
    ct = centroidDecomposition(t, _t_root, _t, B);
    uint32_t time = chrono::duration_cast<chrono::microseconds>(getTime()-t1).count();
    if (check) cerr << "O(n) - " << n << " nodes - correct: " << ((checkCorrectness(t_copy, ct))? "true" : "false") << nl;
    return time;
}

// Settings
uint32_t start = (uint32_t)1e6; // Starting point of benchmark (size)
uint32_t stop = (uint32_t)50e6; // Ending point of benchmark (size)
uint32_t step = (uint32_t)1e6; // Step at every cycle
bool check = false; // Check if every centroid decomposition was correct (very time-consuming!)
uint32_t A = 0; // 'A' parameter
uint32_t B = 0; // 'B' parameter

// Global variables
string tree;
vector<uint32_t> t;

// Main function
int main() {
    for (uint32_t n = start; n <= stop; n += step) {
        uint32_t time_1 = 0, time_2 = 0;
        for (uint32_t i = 0; i < 5; i++) { // Loop 5 times
            // Generate random tree
            system(("tree_gen/random " + to_string(n) + " > tree.txt").c_str()); // Generate random tree
            ifstream in("tree.txt"); in >> tree; in.close(); // Load generated tree
            try {
                t = buildTree(tree); // Build tree (minimal representation)
            } catch (const char* err) {
                cout << err << nl;
                return 0;
            }
            time_1 += nlognCD(t, check); // Perform O(n*log(n)) centroid decomposition
            time_2 += nCD(t, check, A, B); // Perform O(n) centroid decomposition
        }
        time_1 /= 5; time_2 /= 5; // Compute average of 5 times (1 and 2)
        cout << "O(n*log(n)) - " << n << " nodes - time: " << time_1 << " -  formatted: " << printDuration(time_1) << nl; // Output average duration for O(n*log(n)) algorithm
        cout << "O(n) - " << n << " nodes - time: " << time_2 << " - formatted: " << printDuration(time_2) << nl << nl; // Output average duration for O(n) algorithm
        if (!check) cerr << "Done for " << n << " nodes." << nl;
        if (check) cerr << nl;
    }
    system("rm -rf tree.txt"); // Remove 'tree.txt' temporary file
    return 0;
}