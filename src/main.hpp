#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <vector>
#include <stack>
#include <chrono>
#include <algorithm>
#include <cassert>
#include <unistd.h>
#include "utils.cpp"

#define max_deg         0x7fffffff // Max out-degree of the tree
#define max_A           0xffff // Max "A"
#define nl              "\n"
#define pb              push_back
#define iss             istringstream
#define oss             ostringstream
#define num_c           0x7fffffff // Bitmask to extract number of children
#define cov_el          0x80000000 // Bitmask to extract 'cov_el' flag
#define log2(n)         (31 - __builtin_clz(n))