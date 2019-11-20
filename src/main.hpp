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

// Custom stack 
struct stk {

    std::vector<uint32_t> s; // Stack container
    std::uint32_t i; // First free position in the stack (e.g. size of the stack)

    // Initialize the container
    // @param max_size      maximum size the stack will be
    void init(std::size_t max_size) {
        s = std::vector<std::uint32_t>(max_size);
        i = 0;
    }

    // Get the size of the stack
    // @return              size of the stack
    std::size_t size() {
        return i;
    }

    // Check if the stack is empty
    // @return              true if stack is empty, false otherwise
    bool empty() {
        return (size() == 0);
    }

    // Push an element to the stack
    // @param n             element to push
    void push(std::uint32_t n) {
        s[i] = n;
        ++i;
    }

    // Get the top element of the stack
    // @return              top element in the stack
    std::uint32_t top() {
        return s[i-1];
    }

    // Remove an element from the top of the stack
    void pop() {
        --i;
    }
    
};

#endif