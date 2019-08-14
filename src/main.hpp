#include <iostream>
#include <fstream> // Used to read tree structure from file
#include <sstream> // Used to print tree structure
#include <vector> // Used to represent a tree
#include <stack> // Used to visit the trees
#include <chrono> // Used to measure execution times
#include <cmath> // Used only at line 10 of this file
#include "utils.cpp" // Contains all the functions

#define max_deg             pow(2, 31) // Max out-degree of the tree
#define nl                  "\n" // Newline
#define pb                  push_back
#define iss                 istringstream
#define oss                 ostringstream
#define num_c               0x7fffffff // Bitmask to extract number of children
#define cov_el              0x80000000 // Bitmask to extract 'cov_el' flag