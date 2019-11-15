#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <vector>
#include <stack>
#include <chrono>
#include <algorithm>
#include <unistd.h>
#include "utils.cpp"

#ifndef MAIN_HPP
#define MAIN_HPP

#define max_deg         0x7fffffff // Max out-degree of the tree
#define max_A           0xffff // Max "A"
#define max_C           16 // Max "C"
#define nl              "\n"
#define pb              push_back
#define iss             istringstream
#define oss             ostringstream
#define num_c           0x7fffffff // Bitmask to extract number of children
#define cov_el          0x80000000 // Bitmask to extract 'cov_el' flag
#define pow2(n)         (1<<n)
#define log2(n)         (31-__builtin_clz(n))

/**
 * DATA TYPES
 */

// Centroid tree
struct c_tree {
    std::vector<uint8_t> shape;
    std::vector<uint32_t> ids;
};

#endif