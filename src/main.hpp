#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <vector>
#include <chrono>
#include <algorithm>
#include <unistd.h>
#include "utils.cpp"

#ifndef MAIN_HPP
#define MAIN_HPP

#define pb              push_back
#define iss             istringstream
#define oss             ostringstream

constexpr uint32_t max_deg = 0x7fffffff; // Max out-degree of the tree
constexpr uint32_t max_A = 0xffff; // Max "A"
constexpr char nl = '\n';
constexpr uint32_t num_c = 0x7fffffff; // Bitmask to extract number of children
constexpr uint32_t cov_el = 0x80000000; // Bitmask to extract 'cov_el' flag
inline constexpr uint32_t pow2(const uint32_t n) { return (1 << n); }
inline constexpr uint32_t log2(const uint32_t n) { return (31 - __builtin_clz(n)); }
inline uint32_t sizeOfT(const std::vector<uint32_t> &t) { return (t.size() + 2) / 4; }
inline uint32_t sizeOfT2(const std::vector<uint32_t> &t2) { return (t2.size() + 3) / 7; }
inline constexpr uint32_t parnt(const uint32_t i) { return (i + 1); }
inline constexpr uint32_t childOnT(const uint32_t base, const uint32_t i) { return (base + (2 * i) + 2); }
inline constexpr uint32_t sizeOfChildOnT(const uint32_t base, const uint32_t i) { return (childOnT(base, i) + 1); }
inline constexpr uint32_t childOnT2(const uint32_t base, const uint32_t i) { return (base + (3 * i) + 4); }
inline constexpr uint32_t delta1OfChildOnT2(const uint32_t base, const uint32_t i) { return (childOnT2(base, i) + 1); }
inline constexpr uint32_t delta2OfChildOnT2(const uint32_t base, const uint32_t i) { return (childOnT2(base, i) + 2); }
inline constexpr uint32_t alpha(const uint32_t i) { return (i + 3); }

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
    uint32_t i; // First free position in the stack (e.g. size of the stack)

    // Initialize the container
    // @param max_size      maximum size the stack will be
    void init(std::size_t max_size) {
        s = std::vector<uint32_t>(max_size);
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
    void push(uint32_t n) {
        s[i] = n;
        ++i;
    }

    // Get the top element of the stack
    // @return              top element in the stack
    uint32_t &top() {
        return s[i-1];
    }

    // Remove an element from the top of the stack
    void pop() {
        --i;
    }
    
};

#endif