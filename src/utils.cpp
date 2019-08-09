#ifndef UTILS
#define UTILS

#include "main.hpp"
using namespace std;

/*
 * STRUCTURE BUILDING FUNCTIONS
 */

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
            i += 2*cs.top() + 1; cs.pop(); // Increment pointer by 1 + 2 * the number of children of this node
            s.push(id); // Push this node's ID to 's'
        } else { // If this node has no more children
            s.pop(); // Pop its ID from 's'
        }
    }
    return t; // Return 't'
}

// Build a reference bitvector to identify the positions of the nodes in 't'
vector<bool> buildIdRef(const vector<uint32_t> &t) { // Complexity: O(n)
    vector<bool> id_ref = vector<bool>(t.size(), 0); // Initialize an empty bitvector
    uint32_t i = 0; // Initialize vector pointer
    while (i < t.size()) { // While pointer is valid
        id_ref[i] = 1; // Set 'i'th bit to 1
        i += 2*t[i] + 2; // And increment pointer
    }
    return id_ref; // Return 'id_ref'
}

// Build a reference bitvector to identify the positions of the nodes in '_t'
vector<bool> build_IdRef(const vector<uint32_t> &_t) { // Complexity: O(n)
    vector<bool> _id_ref = vector<bool>(_t.size(), 0); // Initialize an empty bitvector
    uint32_t i = 0; // Initialize vector pointer
    while (i < _t.size()) { // While pointer is valid
        _id_ref[i] = 1; // Set 'i'th bit to 1
        i += 3*_t[i] + 4; // And increment pointer
    }
    return _id_ref; // Return '_id_ref'
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

// Cover 't' and build '_t'
pair<uint32_t,vector<uint32_t>> cover(vector<uint32_t> &t, const vector<bool> &id_ref) { // Complexity: O(n)
    vector<uint32_t> _t; // Initialize '_t'
    stack<uint32_t> s, noc, cs; noc.push(0); // 'noc' represents the number of children of each cover element
    uint32_t m = floor(log2((t.size()+2)/4)); // m = log(n)
    uint32_t i = t.size() - 1; // Initialize vector pointer
    uint32_t root; // Initialize root of '_t'
    uint32_t tot_size = (t.size() + 2) / 4; // Number of nodes in 't'
    for (auto it = id_ref.rbegin(); it != id_ref.rend(); it++) { // Visit each element of 'id_ref' in reverse order
        if (*it) { // If the current element on 'id_ref' equals 1 (i.e. there is a node with this ID on 't')
            uint32_t size = 1; // Initialize size
            for (uint32_t j = 0; j < (t[i]&num_c); j++) { // For each children
                if (!(t[t[i+2*j+2]]&cov_el)) { // If child is not a cover element
                    size += s.top(); // Compute its size
                    s.pop();
                } else noc.top()++;
            }
            if (i != 0 && size < m) { // If PCS is not big enough and it's not the root of the tree
                s.push(size); // Push 'size' to 's'
            } else { // If the PCS is big enough or it's the root of the tree
                t[i] |= cov_el; // Mark node as cover element
                // Build '_t'
                uint32_t id = _t.size(); // ID of the new node
                _t.pb(noc.top()); // Number of children
                _t.pb(id); // Parent ID
                _t.pb(size); // Size of treelet
                _t.pb(i); // Treelet root ID reference on 't'
                for (uint32_t j = 0; j < noc.top(); j++) { // For each child
                    _t.pb(cs.top()); // Insert child ID
                    _t.pb(_t[cs.top()+2]); // Initialize delta_1
                    for (uint32_t k = 0; k < _t[cs.top()]; k++) { // For each grandchild
                        _t.back() += _t[cs.top()+3*k+5]; // Compute delta_1
                    }
                    _t.pb(tot_size-_t.back()); // Compute delta_2
                    _t[cs.top()+1] = id; // Then set reference to parent on child
                    cs.pop();
                }
                root = id; // Set latest node as root of '_t'
                cs.push(id); // Push ID of the new node
                noc.push(0);
            }
        }
        i--; // Decrement pointer
    }
    return make_pair(root, _t); // Return bot 'root' and '_t'
}

/*
 * STANDARD O(n*log(n)) CENTROID DECOMPOSITION IMPLEMENTATION
 */

// Remove a node 'n' from 't'
inline void rmNodeOnT(vector<uint32_t> &t, vector<bool> &id_ref, const uint32_t n) { // Complexity: O(k) where k = t[t[n+1]]
    // Remove reference on 'id_ref'
    id_ref[n] = 0;
    // Remove node from 't'
    uint32_t parent = t[n+1]; // Parent of the node ID
    uint32_t size; // Initialize size of the node
    for (uint32_t i = 0; i < (t[parent]&num_c); i++) if (t[parent+2*i+2] == n) size = t[parent+2*i+3]; // Compute size of the node
    // Delete references inside n's parent
    if (parent != n) { // If 'n' has a parent
        for (uint32_t i = 0; i < (t[parent]&num_c); i++) { // Then search among its parent's children
            if (t[parent+2*i+2] == n) {
                uint32_t aux_id = t[parent+2*i+2], aux_size = t[parent+2*i+3]; // And delete the reference to 'n'
                t[parent+2*i+2] = t[parent+2*(t[parent]&num_c)]; t[parent+2*i+3] = t[parent+2*(t[parent]&num_c)+1]; // (i.e. swap its ID and size with the last valid child's ID and size)
                t[parent+2*(t[parent]&num_c)] = aux_id; t[parent+2*(t[parent]&num_c)+1] = aux_size;
                break; // Stop searching
            }
        }
        t[parent]--; // Decrement parent's number of children
        // Update subtree sizes
        uint32_t m = parent; parent = t[m+1]; // Starting from n's parent
        while (m != parent) { // Navigate up the tree
            for (uint32_t i = 0; i < (t[parent]&num_c); i++) if (t[parent+2*i+2] == m) t[parent+2*i+3] -= size; // And update each node's size
            m = parent; parent = t[m+1]; // Then step up
        }
    }
    // Delete references inside n's children
    for (uint32_t i = 0; i < (t[n]&num_c); i++) { // Navigate the children
        t[t[n+2*i+2]+1] = t[n+2*i+2]; // And make them new roots of subtrees
        t[t[n+2*i+2]] |= cov_el; // Then mark them as cover elements
    }
}

// Add a node on '_t'
// Note: when this function is called, the newly added node is ALWAYS the root of a connected component. Therefore, we assume this condition to be true.
inline void addNodeOn_T(vector<uint32_t> &_t, vector<bool> &_id_ref, const uint32_t id_ref, const uint32_t size, const vector<uint32_t> children) { // Complexity: O(k) where k = children.size()
    // Add node on '_t'
    uint32_t id = _t.size(); // ID of the new node
    _t.pb(children.size()); // Number of children
    _t.pb(id); // Parent ID (i.e. itself, see assumption above)
    _t.pb(size); // Size of treelet
    _t.pb(id_ref); // Treelet root ID reference on 't'
    for (uint32_t child : children) { // For each of its children
        _t.pb(child); // Insert child ID
        _t.pb(0); _t.pb(0); // Empty deltas (will be computed by the proper function)
    }
    // Update '_id_ref' consequently
    _id_ref.pb(1);
    for (uint32_t i = 0; i < children.size()+3; i++) {
        _id_ref.pb(0);
    }
}

// Remove a node 'n' from '_t'
inline vector<uint32_t> rmNodeOn_T(vector<uint32_t> &_t, vector<bool> &_id_ref, const uint32_t n) { // Complexity: O(k) where k = _t[_t[n+1]]
    // Remove reference on '_id_ref'
    _id_ref[n] = 0;
    // Remove node from '_t'
    uint32_t parent = _t[n+1]; // Parent of the node ID
    // Delete references inside n's parent
    if (parent != n) { // If 'n' has a parent
        for (uint32_t i = 0; i < _t[parent]; i++) { // The search among its parent's children
            if (_t[parent+3*i+4] == n) {
                uint32_t aux_id = _t[parent+3*i+4], aux_delta_1 = _t[parent+3*i+5], aux_delta_2 = _t[parent+3*i+6]; // And delete the reference to 'n'
                _t[parent+3*i+4] = _t[parent+3*_t[parent]+1]; _t[parent+3*i+5] = _t[parent+3*_t[parent]+2]; _t[parent+3*i+6] = _t[parent+3*_t[parent]+3]; // (i.e. swap its ID and deltas with the last valid child's ID and deltas)
                _t[parent+3*_t[parent]+1] = aux_id; _t[parent+3*_t[parent]+2] = aux_delta_1; _t[parent+3*_t[parent]+3] = aux_delta_2;
                break; // Stop searching
            }
        }
        _t[parent]--; // Decrement parent's number of children
    }
    // Delete references inside n's children
    vector<uint32_t> children; // Initialize children vector
    for (uint32_t i = 0; i < _t[n]; i++) { // Navigate the children
        _t[_t[n+3*i+4]+1] = _t[n+3*i+4]; // And make them new roots of subtrees
        children.pb(_t[n+3*i+4]); // Then add their ID to 'children'
    }
    return children;
}

// Standard centroid search algorithm
inline uint32_t stdFindCentroid(const vector<uint32_t> &t, const uint32_t root) { // Complexity: O(n)
    uint32_t centroid = root; // Start search from root
    if ((t[centroid]&num_c) != 0) { // If the root of the subtree has any children
        uint32_t size = 1; // Initialize size
        for (uint32_t i = 0; i < (t[root]&num_c); i++) size += t[root+2*i+3]; // Compute size
        size /= 2; // Half the size of the tree (but integer!)
        bool found = false;
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
/*
// Standard centroid decomposition algorithm
inline void stdCentroidDecomposition(vector<uint32_t> &t) { // Complexity: O(n*log(n))
    stack<uint32_t> s; s.push(0); // Stack with roots of subtrees yet to process
    // stack<pair<uint32_t,uint32_t>> ps; // TEMP - just used to print nodes
    while (!s.empty()) { // While there are still subtrees to process
        uint32_t root = s.top(); s.pop(); // Get root of subtree
        uint32_t centroid = stdFindCentroid(t, root); // Find centroid of subtree
        rmNodeOnT(t, centroid); // Remove the centroid from T
        uint32_t c = 0; // Subtree counter
        for(int64_t i = (t[centroid]&num_c); i > 0; i--) { // Navigate the children in reverse order
            s.push(t[centroid+2*i]); // Then push them to 's'
            c++; // And increment counter
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
//     }
// }

/*
 * NEW O(n) CENTROID DECOMPOSITION IMPLEMENTATION
 */

// Recompute the deltas on a treelet (i.e. a connected component of '_t')
inline void computeDeltas(const vector<uint32_t> &t, vector<uint32_t> &_t, const uint32_t n) { // Complexity: O(log(n))
    // Identify root of treelet
    uint32_t root = n;
    uint32_t parent = _t[root+1];
    while (parent != root) { // Navigate up the treelet until the root is found
        root = parent; parent = _t[root+1]; // Step up
    }
    // Compute total size of treelet
    uint32_t size = 1; // Initialize size of connected component rooted at 'root'
    for (uint32_t i = 0; i < (t[_t[root+3]]&num_c); i++) {
        size += t[_t[root+3]+2*i+3]; // Compute size
    }
    // Build stack for DFS
    stack<uint32_t> s; s.push(root); // Initialize stack for DFS
    stack<uint32_t> dfs;
    while (!s.empty()) { // While stack is not empty
        uint32_t node = s.top(); s.pop(); // ID of the node being visited
        dfs.push(node); // Push node to DFS stack
        for (uint32_t i = _t[node]; i > 0; i--) { // For each child
            s.push(_t[node+3*i+1]); // Push its ID into the stack
        }
    }
    // DFS on '_t' and compute deltas
    while (!dfs.empty()) { // While DFS stack is not empty
        uint32_t node = dfs.top(); dfs.pop(); // ID of the node being visited
        for (uint32_t i = 0; i < _t[node]; i++) { // For each of its children
            _t[node+3*i+5] = _t[_t[node+3*i+4]+2]; // Initialize delta_1
            for (uint32_t j = 0; j < _t[_t[node+3*i+4]]; j++) _t[node+3*i+5] += _t[_t[node+3*i+4]+3*j+5]; // Compute delta_1
            _t[node+3*i+6] = size - _t[node+3*i+5]; // Compute delta_2
        }
    }
}

// New centroid search algorithm
inline uint32_t findCentroid(const vector<uint32_t> &t, const vector<uint32_t> &_t, const uint32_t root) { // Complexity: ?? -> should be O(n/log(n))
    // TODO
    return 0;
}

// New centroid decomposition algorithm
void centroidDecomposition(vector<uint32_t> &t, const uint32_t _t_root, vector<uint32_t> &_t) { // Complexity: ?? -> should be O(n)
    // TODO
}

/*
 * UTILS
 */

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