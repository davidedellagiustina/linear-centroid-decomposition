#ifndef UTILS
#define UTILS

#include "main.hpp"
using namespace std;

// Build tree structure from balanced parenthesis representation
vector<uint32_t> buildTree(const string &tree) { // Complexity: O(n)
    stack<uint32_t> s, cs;
    // Build vector of number of children: O(n)
    uint32_t n = 1; // Number of nodes = 1 + number of total children
    for (int64_t i = tree.size()-1; i >= 0; i--) { // Read string in reverse order
        if (tree[i] == ')') {
            if (!s.empty()) s.top()++; // Add one child to parent
            s.push(0); // New node with 0 children (up to now)
        } else {
            uint32_t k = s.top(); s.pop(); // Remove the number of children of the last node added
            if (k > max_deg) throw "Tree is too big: out-degree overflow."; // Check for out-degree overflow
            n += k; // Add 'k' to 'n'
            cs.push(k); // Push this children's number of nodes to 'cs'
        }
    }
    n = 4*n - 2; // Number of vector elements (linear!)
    // Build tree representation: O(n)
    vector<uint32_t> t = vector<uint32_t>(n, 0); // Initialize an empty vector for the tree
    uint32_t i = 0; // Vector pointer
    for (char c : tree) { // Read the string representing tree structure
        if (c == '(') {
            uint32_t id = i; // ID of this node
            i += 1; // Increment the pointer by 1 (this cell represents the current node's parent ID)
            if (!s.empty()) { // If parent exists
                t[i] = s.top(); // Write it in its cell
                t[s.top()]++; // Increment parent's number of children
                t[s.top() + 2*t[s.top()]] = id; // Write this node's ID among its parent's children (in the first empty position)
            } else { // If this node has no parent
                t[i] = id; // Write the same ID on parent ID cell (meaning no parent)
            }
            i += 2*cs.top() + 1; cs.pop(); // Increment pointer by 1 + the number of children of this node
            s.push(id); // Push this node's ID to 's'
        } else { // If this node has no more children
            s.pop(); // Pop its ID from 's'
        }
    }
    return t;
}

// Build a reference bitvector to identify the positions of the nodes in 't'
vector<bool> buildIdRef(const vector<uint32_t> &t) { // Complexity: O(n)
    vector<bool> id_ref = vector<bool>(t.size(), 0); // Initialize an empty bitvector
    uint32_t i = 0; // Initialize vector pointer
    while (i < t.size()) { // While pointer is valid
        id_ref[i] = 1; // Set 'i'th bit to 1
        i += 2*t[i] + 2; // And increment pointer
    }
    return id_ref;
}

// Compute the initial sizes of the tree
void computeSizes(vector<uint32_t> &t, const vector<bool> &id_ref) { // Complexity: O(n)
    uint32_t i = t.size() - 1; // Initialize vector pointer
    for (auto it = id_ref.rbegin(); it != id_ref.rend(); it++) { // Visit each element of 'id_ref' in reverse order
        if (*it) { // If the current element on 'id_ref' equals 1 (i.e. there is a node with this ID on 't')
            for (uint32_t j = 0; j < (t[i]&num_c); j++) { // For each of its children
                uint32_t child = t[i+2*j+2]; // ID of the child
                uint32_t size = 1; // Initial size of the child
                for (uint32_t k = 0; k < (t[child]&num_c); k++) size += t[child+2*k+3]; // Compute the size of the child
                t[i+2*j+3] = size; // Write the size on 't'
            }
        }
        i--; // Decrement pointer
    }
}

// Cover T and build T"
vector<uint32_t> cover(vector<uint32_t> &t, vector<bool> &id_ref) { // Complexity: ??
    vector<uint32_t> tii; // Initialize T"
    stack<uint32_t> s;
    uint32_t m = floor(log2((t.size()+2)/4)); // m = log(n)
    uint32_t i = t.size() - 1; // Initialize vector pointer
    for (auto it = id_ref.rbegin(); it != id_ref.rend(); it++) { // Visit each element of 'id_ref' in reverse order
        if (*it) { // If the current element on 'id_ref' equals 1 (i.e. there is a node with this ID on 't')
            uint32_t size = 1; // Initialize size
            for (uint32_t j = 0; j < (t[i]&num_c); j++) { // For each children
                uint32_t child = t[i+2*j+2]; // ID of the child
                if (!(t[child]&cov_el)) { // If child is not a cover element
                    size += s.top(); // Compute its size
                    s.pop();
                }
            }
            if (i != 0 && size < m) {
                s.push(size); // Push 'size' to 's'
            } else {
                t[i] |= cov_el; // Mark node as cover element
                cout << "Node " << i << ": size " << size << nl;
            }
        }
        i--; // Decrement pointer
    }
    return tii;
}

// Remove a node 'n' from T
inline void rmNode(vector<uint32_t> &t, uint32_t n) { // Complexity: O(k) where k is n's parent number of children
    uint32_t parent = t[n+1]; // Parent of the node ID
    uint32_t size; // Initialize size of the node
    for (uint32_t i = 0; i < (t[parent]&num_c); i++) if (t[parent+2*i+2] == n) size = t[parent+2*i+3]; // Compute size of the node
    // Delete references inside n's parent
    if (parent != n) { // If 'n' has a parent
        for (uint32_t i = 0; i < (t[parent]&num_c); i++) { // Then search among its parent's children
            if (t[parent+2*i+2] == n) {
                uint32_t aux_id = t[parent+2*i+2]; uint32_t aux_size = t[parent+2*i+3]; // And delete the reference to 'n'
                t[parent+2*i+2] = t[parent+2*(t[parent]&num_c)]; t[parent+2*i+3] = t[parent+2*(t[parent]&num_c)+1]; // (i.e. swap its ID and size with the last valid child's ID and size)
                t[parent+2*(t[parent]&num_c)] = aux_id; t[parent+2*(t[parent]&num_c)+1] = aux_size;
                break; // Stop searching
            }
        }
        t[parent]--; // Decrement parent's number of children
        // Delete references inside n's children
        if ((t[n]&num_c) > 0) { // If the node has any children
            for (uint32_t i = 0; i < (t[n]&num_c); i++) { // Then navigate them
                t[t[n+2*i+2]+1] = t[n+2*i+2]; // And make them new roots of subtrees
            }
        }
        // Update subtree sizes
        uint32_t m = parent; parent = t[m+1]; // Starting from n's parent
        while (m != parent) { // Navigate up the tree
            for (uint32_t i = 0; i < (t[parent]&num_c); i++) if (t[parent+2*i+2] == m) t[parent+2*i+3] -= size; // And update each node's size
            m = parent; parent = t[m+1]; // Then step up
        }
    }
}

// Standard centroid search algorithm
inline uint32_t stdFindCentroid(const vector<uint32_t> &t, const uint32_t root) { // Complexity: O(n)
    uint32_t size = 1; // Initialize size
    for (uint32_t i = 0; i < (t[root]&num_c); i++) size += t[root+2*i+3]; // Compute size
    size /= 2; // Half the size of the tree (but integer!)
    uint32_t centroid = root; // Start search from root
    bool found = false;
    if ((t[centroid]&num_c) != 0) {
        while (!found) { // Loop until the centroid is found
            for (uint32_t i = 0 ; i < (t[centroid]&num_c); i++) { // For each children
                if (t[centroid+2*i+3] > size) { // If the subtree rooted at this child is bigger than 'size'
                    centroid = t[centroid+2*i+2]; // Then search centroid in that direction
                    break; // And stop visiting children
                }
                found = true;
            }
        }
    }
    return centroid;
}

// Standard centroid decomposition algorithm
inline void stdCentroidDecomposition(vector<uint32_t> t) { // Complexity: O(n*log(n))
    stack<uint32_t> s; s.push(0); // Stack with roots of subtrees yet to process
    stack<pair<uint32_t,uint32_t>> ps; // TEMP - just used to print nodes
    while (!s.empty()) { // While there are still subtrees to process
        uint32_t root = s.top(); s.pop(); // Get root of subtree
        uint32_t centroid = stdFindCentroid(t, root); // Find centroid of subtree
        rmNode(t, centroid); // Remove the centroid from T
        uint32_t c = 0; // Subtree counter
        if ((t[centroid]&num_c) > 0) { // If the centroid has some children
            for(int64_t i = (t[centroid]&num_c); i > 0; i--) { // Navigate them in reverse order
                s.push(t[centroid+2*i]); // Then push them to 's'
                c++; // And increment counter
            }
        }
        if (centroid != root) { // If the root of the subtree is not its centroid
            s.push(root); // Then it is the root of a new subtree
            c++; // Increment counter
        }
        // TEMP - prints all the nodes
        /* UNCOMMENT TO PRINT CENTROID TREE
        cout << "Node " << centroid << " "; // Print node ID
        if (!ps.empty()) { // If node has parent
            cout << "has parent " << ps.top().second << "." << nl; // Print parent ID
            ps.top().first--; // Decrement parent's number of children
            if (ps.top().first == 0) ps.pop(); // If parent has no more children, remove if from 'ps'
        } else { // If node has no parent
            cout << "is the root of the tree." << nl; // Preint that is is the root of the centroid tree
        }
        if (c != 0) ps.push(make_pair(c, centroid)); // If this node will have some children (i.e. when removed it generates subtrees on T), then push it to 'ps'
        */
    }
}

// Get time
inline chrono::high_resolution_clock::time_point getTime() { // Complexity: O(1)
    return chrono::high_resolution_clock::now(); // Get new time reference and return
}

// Print elapsed time
inline string printTime(const string t, const chrono::high_resolution_clock::time_point t1, const chrono::high_resolution_clock::time_point t2) { // Complexity: O(1)
    auto duration = chrono::duration_cast<chrono::microseconds>(t2-t1).count(); // Compute duration
    int us = duration; // Microseconds
    int ms = us/1000; us %= 1000; // Milliseconds
    int s = ms / 1000; ms %= 1000; // Seconds
    int m = s/60; s %= 60; // Minutes
    int h = m/60; m %= 60; // Hours
    oss os; // New output stream
    os << t << " time: " << h << "h " << m << "m " << s << "s " << ms << "ms " << us << "us"; // Print to 'os'
    return os.str(); // Return stream content as a string
}

// Print a vector<uint32_t>
inline string print(const vector<uint32_t> &t) { // Complexity: O(n)
    oss os; // New output stream
    os << "(";
    bool first = true; // Used for the first element
    for (const uint32_t i : t) { // Print each node to 'os'
        if (first) {
            os << i;
            first = false;
        } else os << " " << i;
    }
    os << ")";
    return os.str(); // Return stream content as a string
}

#endif