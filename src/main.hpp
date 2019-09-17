// Used libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <vector>
#include <stack>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <unistd.h>
#include "utils.cpp"

// Macros
#define max_deg             pow(2, 31) // Max out-degree of the tree
#define nl                  "\n" // Newline placeholder
#define pb                  push_back
#define iss                 istringstream
#define oss                 ostringstream
#define num_c               0x7fffffff // Bitmask to extract number of children
#define cov_el              0x80000000 // Bitmask to extract 'cov_el' flag