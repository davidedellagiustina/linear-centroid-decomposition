#ifndef UTILS
#define UTILS

#include "main.hpp"
using namespace std;

/*
 * STRUCTURE BUILDING FUNCTIONS
 */

// Build tree structure from balanced parenthesis representation, level-wise, in-place
// @param tree: BP representation of tree
// @return tree structure
vector<uint32_t> buildTree(const string &tree) { // Complexity: O(n)
    uint32_t n = tree.length() / 2; // Number of nodes
    uint32_t N = 4*n - 2; // Size of 't'
    vector<uint32_t> t = vector<uint32_t>(N); // Initialize empty 't'
    uint32_t H = 0; // Max height
    // Compute the number of nodes per level in t[0...H-1]
    int64_t h = 1; // Current height
    for (uint32_t i = 0; i < tree.length(); i++) {
        t[h-1] += (tree[i] == '(');
        h += ((tree[i] == '(')? 1 : -1);
        H = std::max(uint32_t(h), H); // Also compute max height
    }
    // Compute the partial sums in t[0...H-1]
    uint32_t psum = 0, tmp;
    for (uint32_t i = 0; i < H; i++) {
        tmp = t[i];
        t[i] = psum;
        psum += tmp;
    }
    // Count the number of children for each node. We store this info in t[H...H+n-1]
    h = 0;
    for (uint32_t i = 1; i < tree.length(); i++) {
        t[H+t[h]] += (tree[i] == '(');
        if (t[H+t[h]] >= max_deg) throw "Tree is too big: out-degree overflow."; // Check for out-degree overflow
        t[h] += (tree[i] == ')');
        h += ((tree[i] == '(')? 1 : -1);
    }
    // Left-shift t[H...H+n-1] in t[0...n-1]
    for (uint32_t i = 0; i < n; i++) t[i] = t[i+H];
    uint32_t r = n; // t[r] = number of children of last allocated node
    uint32_t R = N; // Start position (=ID) in t of last allocated node
    for(uint32_t i = 0; i < n; i++) { // For all nodes
        r--; // Process next node (right to left)
        R -= 2 + 2*t[r]; // Node ID
        t[R] = t[r]; // Number of children
        // for (int j = 0; j < 1+2*t[r]; j++) t[R+1+j] = 0; // Set to 0 all values but number of children (not needed it seems)
    }
    // Set parent/children pointers
    uint32_t i = 0; // Pointer to current node X in current level
    uint32_t j = t[0]*2 + 2; // Pointer to child of X in next level
    t[1] = 0; // Parent of root = root
    while (j < N) {
        uint32_t x = i; // This node
        uint32_t n_c = t[x]; // Number of children of this node
        i += 2; // Jump to children area
        for (int k = 0; k < n_c; k++) {
            t[i] = j; // Write child's ID
            i += 2; // Jump to next child 
            t[j+1] = x; // Store parent of j
            j += 2*t[j]+2; // Jump to next child's area
        }
    }
    return t; // Return 't'
}

// Build a reference bitvector to identify the positions of the nodes n 't'
// @param t: tree structure
// @return reference bitvector
vector<bool> buildIdRef(const vector<uint32_t> &t) { // Complexity: O(n)
    vector<bool> id_ref = vector<bool>(t.size(), 0); // Initialize an empty bitvector
    uint32_t i = 0; // Initialize vector pointer
    while (i < t.size()) { // While pointer is valid
        id_ref[i] = 1; // Set 'i'-th bit to 1
        i += 2*(t[i]&num_c)+2; // And increment pointer
    }
    return id_ref; // Return 'id_ref'
}

// Compute the initial sizes of the tree
// @param t: tree structure
// @param id_ref: reference bitvector
void computeSizes(vector<uint32_t> &t, vector<bool> &id_ref) { // Complexity: O(n)
    uint32_t i = t.size() - 2; // Initialize vector pointer (last node has ID 't.size()-2')
    uint32_t p = t.size(); // Parent of current node (invalid at the beginning)
    uint32_t nc = 0; // Current node is the 'nc'-th child of its parent
    while (i > 0) { // While pointer is valid
        if (id_ref[i]) { // If the current element on 'id_ref' equals 1 (i.e. there is a node with this ID on 't')
            // assert(t[i+1] != p or nc > 0);
            // assert((t[t[i+1]]&num_c) > 0);
            nc = ((t[i+1] != p)? (t[t[i+1]]&num_c)-1 : nc-1); // Update child's number
            p = t[i+1]; // Update parent
            uint32_t size = 1; // Size of subtree rooted at 'i'
            for (uint32_t j = 0; j < (t[i]&num_c); j++) // For each of its children
                size += t[i+2*j+3]; // Size of the child
            t[p+3+nc*2] = size; // Set size
        }
        i--; // Decrement pointer
    }
}

// Cover 't' and build '_t'
// @param t: tree structure
// @param A: minimum size of cover elements - log(n) if not given
// @return '_t' and its root ID
pair<uint32_t,vector<uint32_t>> cover_old(vector<uint32_t> &t, uint32_t A = 0) { // Complexity: O(n)
    uint32_t n = (t.size()+2)/4; // Number of nodes
    vector<uint32_t> _t; // Initialize '_t'
    if (!A) A = floor(log2(n)); // If 'A' is not given
    uint32_t i = 0, p = t[i+1], root;
    stack<uint32_t> nc, sizes, pcs_c, noc;
    nc.push(t[i]&num_c); sizes.push(0); noc.push(0);
    while (i != 0 || !nc.empty()) {
        if (nc.top() > 0) { // If node 'i' has still children to visit
            i = t[i+2*nc.top()]; p = t[i+1]; // Visit 'nc.top()'-th children of 'i'
            nc.top()--; // Decrement number of children yet to visit
            nc.push(t[i]&num_c); sizes.push(0); noc.push(0);
        } else { // If all 'i' children have been visited
            nc.pop();
            uint32_t size = sizes.top() + 1; sizes.pop(); // Size of PCS
            uint32_t c = noc.top(); noc.pop(); // Number of children on '_t'
            if (size >= A || i == 0) {  // If PCS is big enough or it is the root of the tree (i.e. node 0)
                t[i] |= cov_el; // Mark it as cover element
                uint32_t id = _t.size(); // ID on '_t'
                _t.pb(c); // Number of children
                _t.pb(id); // Parent ID
                _t.pb(size); // Size of treelet
                _t.pb(i); // Treelet root ID reference on 't'
                for (uint32_t j = 0; j < c; j++) { // For each of its children
                    _t[pcs_c.top()+1] = id; // Update child's parent
                    _t.pb(pcs_c.top()); // Child ID
                    _t.pb(_t[pcs_c.top()+2]); // Initialize delta_1
                    for (uint32_t k = 0; k < _t[pcs_c.top()]; k++) _t.back() += _t[pcs_c.top()+3*k+5]; // Compute delta_1
                    _t.pb(n-_t.back()); // Delta_2
                    pcs_c.pop();
                }
                root = id; pcs_c.push(id); // Last node added on '_t' is its root
                if (!noc.empty()) noc.top()++;
            } else { // If PCS is smaller than 'A'
                sizes.top() += size; noc.top() += c;
            }
            i = p; p = t[i+1]; // Navigate to parent
        }
    }
    return make_pair(root, _t); // Return both '_t' and its root ID
}

// Cover 't' and build '_t'
// Note: 'computeSizes()' souldn't be called: this procedure already computes those sizes
// @param t: tree structure
// @param id_ref: reference bitvector
// @param A: minimum size of cover elements - log(n) if not given
// @return '_t'
vector<uint32_t> cover(vector<uint32_t> &t, const vector<bool> &id_ref, uint32_t A = 0) { // Complexity: O(n)
    uint32_t n = (t.size()+2)/4; // Number of nodes of 't'
    if (!A) A = floor(log2(n)); // If 'A' is not given
    uint32_t k = ceil(n/A)+1; // Upper-bound of '_t' nodes
    vector<uint32_t> X = vector<uint32_t>(n);
    vector<tuple<uint32_t,uint32_t,uint32_t,uint32_t>> q = vector<tuple<uint32_t,uint32_t,uint32_t,uint32_t>>(k); // Fields: depth, pre_ord, size, t_node
    uint32_t q_ptr = q.size()-1;
    // Step 1 - bottom-up visit: O(n)
    int64_t i = t.size() - 1;
    uint32_t p = t.size(), nc = 0;
    auto x = (uint8_t*)X.data();
    uint32_t x1_ptr = 0, x2_ptr = 0; // 'x1_ptr' is at level L, 'x2_ptr' is at level L+1
    while (i >= 0) {
        if (id_ref[i]) {
            nc = ((t[i+1] != p)? (t[t[i+1]]&num_c)-1 : nc-1);
            p = t[i+1];
            // Compute total and partial sizes
            t[p+2*nc+3] = ((i != 0)? 1 : t[p+2*nc+3]);
            uint32_t size = 1;
            for (uint32_t j = 0; j < (t[i]&num_c); j++) {
                t[p+2*nc+3] += ((i != 0)? t[i+2*j+3] : 0);
                size += x[x2_ptr];
                x2_ptr++;
            }
            // Create cover element
            if (size >= A || i == 0) {
                t[i] |= cov_el;
                std::get<2>(q[q_ptr]) = size;
                std::get<3>(q[q_ptr]) = i;
                q_ptr--;
            } else x[x1_ptr] = size;
            // Increment pointers
            x1_ptr++;
        }
        i--;
    }
    // Step 2 - top-down visit: O(n)
    i = 0; p = 0; q_ptr++;
    X[0] = 0;
    uint32_t p_depth = 0, pre_ord, X0_ptr = 0, X1_ptr = 0, X2_ptr = 1;
    while (i < t.size()) {
        k = p + 2*(t[p]&num_c) + 2;
        X0_ptr += ((t[i+1] != p)? ((t[i+1] == k)? 1 : ((t[i+1]-k)/2)+1) : 0);
        p_depth = ((t[i+1] != p || t[i+1] == 0)? X[X0_ptr] : p_depth);
        p = t[i+1];
        pre_ord = X[X1_ptr];
        // Save 'pre_ord' for node 'i' if it is marked as cover element
        uint32_t z = 0;
        if (t[i]&cov_el) {
            z = 1;
            std::get<1>(q[q_ptr]) = pre_ord;
        }
        // Compute and save 'pre_ord' for i's children
        uint32_t s = 1 + pre_ord;
        for (uint32_t j = 0; j < (t[i]&num_c); j++) {
            X[X2_ptr] = s;
            s += t[i+2*j+3];
            X2_ptr++;
        }
        // Compute and save 'depth' for node 'i'
        X[X1_ptr] = p_depth + z;
        if (t[i]&cov_el) {
             std::get<0>(q[q_ptr]) = X[X1_ptr];
             q_ptr++;
        }
        // Update variables
        X1_ptr++;
        i += 2*(t[i]&num_c)+2;
    }
    // Step 3 - build basic T'': O(n/log(n))
    std::sort(q.begin(), q.end());
    uint32_t q1_ptr = 0, q2_ptr; while (std::get<2>(q[q1_ptr]) == 0) q1_ptr++; q2_ptr = q1_ptr + 1;
    uint32_t m = q.size() - q1_ptr; // Number of nodes of '_t'
    vector<uint32_t> _t = vector<uint32_t>(7*m-3);
    i = 0;
    while (i < _t.size()) {
        _t[i+2] = std::get<2>(q[q1_ptr]);
        _t[i+3] = std::get<3>(q[q1_ptr]);
        i += 4;
        nc = 0;
        if (q1_ptr+1 < q.size()) {
            if (std::get<0>(q[q1_ptr+1]) > std::get<0>(q[q1_ptr])) {
                uint32_t l = std::get<0>(q[q1_ptr+1]);
                while (q2_ptr < q.size() && std::get<0>(q[q2_ptr]) == l) {
                    nc++; q2_ptr++;
                    i += 3;
                }
            } else {
                while (std::get<1>(q[q2_ptr]) < std::get<1>(q[q1_ptr+1])) {
                    nc++; q2_ptr++;
                    i += 3;
                }
            }
        }
        _t[i-3*nc-4] = nc;
        q1_ptr++;
    }
    // Step 4 - compute parent-children pointers: O(n/log(n))
    i = 0;
    uint32_t j = 3*_t[i]+4;
    while (i < _t.size()) {
        for (uint32_t k = 0; k < _t[i]; k++) {
            _t[i+3*k+4] = j; _t[j+1] = i;
            j += 3*_t[j]+4;
        }
        i += 3*_t[i]+4;
    }
    for (auto el : _t) cout << el << " "; cout << nl;
    return _t;
}

/*
 * STANDARD O(n*log(n)) CENTROID DECOMPOSITION IMPLEMENTATION
 */

// Remove a node 'n' from 't'
// @param t: tree structure
// @param n: ID of note to be removed
inline void rmNodeOnT(vector<uint32_t> &t, const uint32_t n) { // Complexity: O(k) where k = t[t[n+1]]
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
    }
}

// Add a node on '_t'
// Note: when this function is called, the newly added node is ALWAYS the root of a connected component. Therefore, we assume this condition to be true.
// @param _t: '_t' tree structure
// @param ref: reference ID on 't'
// @param size: size of treelet
// @param children: vector of the children of the new node
// @return ID of the newly added node
inline uint32_t addNodeOn_T(vector<uint32_t> &_t, const uint32_t ref, const uint32_t size, const vector<uint32_t> &children) { // Complexity: O(k) where k = children.size()
    // Add node on '_t'
    uint32_t id = _t.size(); // ID of the new node
    _t.pb(children.size()); // Number of children
    _t.pb(id); // Parent ID (i.e. itself, see assumption above)
    _t.pb(size); // Size of treelet
    _t.pb(ref); // Treelet root ID reference on 't'
    for (uint32_t child : children) { // For each of its children
        _t.pb(child); // Insert child ID
        _t.pb(0); _t.pb(0); // Empty deltas (will be computed by the proper function)
        _t[child+1] = id; // Update child's parent ID
    }
    return id; // Return the ID of the newly addd node
}

// Remove a node 'n' from '_t'
// @param _t: '_t' tree structure
// @param n: ID of the node to be removed
// @return vector of the children of the removed node
inline vector<uint32_t> rmNodeOn_T(vector<uint32_t> &_t, const uint32_t n) { // Complexity: O(k) where k = _t[_t[n+1]]
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
    return children; // Return 'children'
}

// Standard centroid search algorithm
// @param t: tree structure
// @param root: root of the connected component
// @return centroid of the connected component
inline uint32_t stdFindCentroid(const vector<uint32_t> &t, const uint32_t root) { // Complexity: O(n)
    // Compute size of subtree: O(k) where k = (t[root]&num_c)
    uint32_t size = 1; // Initialize size
    for (uint32_t i = 0; i < (t[root]&num_c); i++) size += t[root+2*i+3]; // Compute size
    size /= 2; // Half the size of the tree (but integer!)
    // Search centroid: O(n)
    uint32_t centroid = root; // Start search from root
    bool found = false; // Initialize 'found'
    if ((t[centroid]&num_c) > 0) { // If the root of the subtree has any children
        while (!found) { // Loop until the centroid is found
            for (uint32_t i = 0 ; i < (t[centroid]&num_c); i++) { // For each children
                if (t[centroid+2*i+3] > size) { // If the subtree rooted at this child is bigger than 'size'
                    centroid = t[centroid+2*i+2]; // Then search centroid in that direction
                    found = false;
                    break; // And stop visiting children
                } else {
                    found = true;
                }
            }
        }
    }
    return centroid; // Return the ID of the centroid
}

// Standard centroid decomposition algorithm
// @param t: tree structure
// @param root: root of the tree/connected component
// @param N: number of nodes of the tree to elaborate
// @return vector representation of the centroid tree
inline vector<uint32_t> stdCentroidDecomposition(vector<uint32_t> &t, const uint32_t root = 0, uint32_t N = 0) { // Complexity: O(n*log(n))
    if (!N) N = (t.size()+2)/4; // Number of nodes
    vector<uint32_t> out = vector<uint32_t>(3*N, 0); // Initialize output vector
    uint32_t pos = 0; // Output vector pointer
    stack<uint32_t> s; s.push(root); // Stack with roots of subtrees yet to process
    while (!s.empty()) { // While there are still subtrees to process
        uint32_t r = s.top(); s.pop(); // Get root of subtree
        uint32_t centroid = stdFindCentroid(t, r); // Find centroid of subtree
        uint32_t c = 0; for (uint32_t i = 0; i < (t[centroid]&num_c); i++) c += t[centroid+2*i+3]; // Total size of centroid subtrees
        rmNodeOnT(t, centroid); // Remove the centroid from T
        for(uint32_t i = (t[centroid]&num_c); i > 0; i--) // Navigate the children in reverse order
            s.push(t[centroid+2*i]); // Then push them to 's'
        if (centroid != r) { // If the root of the subtree is not its centroid
            s.push(r); // Then it is the root of a new subtree
            c++; for (uint32_t i = 0; i < (t[r]&num_c); i++) c += t[r+2*i+3]; // Size of root subtree
        }
        // Print the current node to the output vector
        while (out[pos] == 1) pos++;
        out[pos] = 0; // Print "("
        out[pos+1] = centroid + 2; // Print centroid ID
        pos += 2; // Update 'pos'
        out[pos+3*c] = 1; // Print "("
    }
    return out;
}

/*
 * NEW O(n) CENTROID DECOMPOSITION IMPLEMENTATION
 */

// Recompute the deltas on a treelet (i.e. a connected component of '_t')
// @param t: tree structure
// @param _t: '_t' tree structure
// @param root: root of the connected component of which to compute deltas
inline void computeDeltas(const vector<uint32_t> &t, vector<uint32_t> &_t, const uint32_t root) { // Complexity: O(log(n))
    // Compute total size of treelet
    uint32_t size = 1; // Initialize size of connected component rooted at 'root'
    for (uint32_t i = 0; i < (t[_t[root+3]]&num_c); i++) size += t[_t[root+3]+2*i+3]; // Compute size
    // Build stack for DFS
    stack<uint32_t> s, dfs; s.push(root); // Initialize stack for DFS
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
// @param t: tree structure
// @param _t: '_t' tree structure
// @param root: root of the connected component
// return centroid of the connected component (both IDs on 't' and '_t')
inline pair<uint32_t,uint32_t> findCentroid(const vector<uint32_t> &t, const vector<uint32_t> &_t, const uint32_t root) { // Complexity: O(n/log(n))
    // Compute size of subtree (i.e. connected component): O(1)
    uint32_t size = ((_t[root] == 0)? _t[root+2] : _t[root+5]+_t[root+6]); // Compute size
    size /= 2; // Compute size
    // Serach centroid treelet on '_t': O(n/log(n))
    uint32_t centroid_treelet = root; // Start searching from root
    bool found = false; // Initialize 'found'
    while (!found) { // While the centroid treelet is not found
        if (_t[centroid_treelet] > 0) { // If 'centroid_treelet' has any children
            for (uint32_t i = 0; i < _t[centroid_treelet]; i++) { // Navigate the children of the current node
                if (_t[centroid_treelet+3*i+5] > size) { // If it is the heavy child
                    centroid_treelet =_t[centroid_treelet+3*i+4]; // Move towards it
                    found = false; // And set 'found' to false
                    break; // Then stop navigating children
                } else { // Else
                    found = true; // Set 'found' to true
                }
            }
        } else found = true;
    }
    // Search centroid node on 't': O(log(n))
    uint32_t centroid_node = _t[centroid_treelet+3]; // Start searching from centroid_treelet's root
    found = false; // Reset 'found'
    if ((t[centroid_node]&num_c) > 0) { // if 'centroid_node' has any children
        while (!found) { // While the centroid node is not found
            for (uint32_t i = 0; i < (t[centroid_node]&num_c); i++) { // Navigate the children of the current node
                if (t[centroid_node+2*i+3] > size) { // If it is the heavy child
                    centroid_node = t[centroid_node+2*i+2]; // Move towards it
                    found = false; // And set 'found' to false
                    break; // Then stop navigating children
                } else { // Else
                    found = true; // Set 'found' to true
                }
            }
        }
    }
    return make_pair(centroid_treelet, centroid_node); // Return the ID of the centroid node both on 't' and '_t'
}

// New centroid decomposition algorithm
// @param t: tree structure
// @param _t: '_t' tree structure
// @param B: maximum size when to apply standard centroid decomposition - (log(n))^3 if not given
// @return vector representation of the centroid tree
vector<uint32_t> centroidDecomposition(vector<uint32_t> &t, vector<uint32_t> &_t, uint32_t B = 0) { // Complexity: O(n)
    uint32_t N = (t.size()+2)/4; // Nuber of nodes
    if (!B) B = pow(floor(log2(N)), 3); // If 'B' is not given
    vector<uint32_t> out = vector<uint32_t>(3*N, 0); // Initialize output vector
    uint32_t pos = 0; // Output vector pointer
    stack<uint32_t> s, noc; s.push(0); // Stack with roots of subtrees yet to process
    while (!s.empty()) { // While stack is not empty
        uint32_t root = s.top(); s.pop(); // Get root from stack
        uint32_t dimens = 1; // Initialize 'dimens'
        for (uint32_t i = 0; i < (t[_t[root+3]]&num_c); i++) dimens += t[_t[root+3]+2*i+3]; // Compute 'dimens'
        if (dimens > B) { // If 'dimens' is bigger than 'B'
            computeDeltas(t, _t, root); // COmpute deltas of connected component
            pair<uint32_t,uint32_t> centroid = findCentroid(t, _t, root); // Find centroid of subtree with root 'root'
            uint32_t _tc = centroid.first, tc = centroid.second; // Parse centroid nodes on 't' and '_t'
            rmNodeOnT(t, tc); // Remove node 'tc' from 't'
            vector<uint32_t> children = rmNodeOn_T(_t, _tc); // Remove node '_tc' from '_t' and get its children on '_t'
            // Build children reference vector
            vector<pair<uint32_t,uint32_t>> c_ref; // Initialize vector
            for (uint32_t child : children) { // For each child
                uint32_t n = _t[child+3], p = t[n+1]; // 'n' is the root of 'child' on 't', 'p' is n's parent
                while (p != n) { // Navigate up the tree to find root
                    n = p; p = t[n+1]; // Step up
                }
                c_ref.pb(make_pair(n, child)); // Push element to 'c_ref'
            }
            // Build new nodes on '_t' for each 'tc's children
            uint32_t nc = (t[tc]&num_c); // Number of children - used when printing centroid tree representation
            uint32_t total_number = 0; // Total number of old '_t' nodes whose parent has been found - used when updating 'tc's parent
            uint32_t total_size = 1; // Total size of the newly created nodes on '_t' - used when updating 'tc's parent
            for (uint32_t i = nc; i > 0; i--) { // For each children, in reverse order
                uint32_t new_node; // Initialize 'new_node'
                uint32_t child = t[tc+2*i]; // Child ID
                if (!(t[child]&cov_el)) { // If child is not a cover element
                    t[child] |= cov_el; // Mark it as a new cover element
                    uint32_t size = t[tc+2*i+1]; // Initialize its size
                    total_size += size; // Increment 'total_size'
                    vector<uint32_t> c; // Initialize its children vector
                    for (pair<uint32_t,uint32_t> node : c_ref) { // For each node in 'c_ref' (i.e. a node on '_t' whose new parent has to be found)
                        if (child == node.first) { // If its new parent is the new node being created
                            c.pb(node.second); // Push its ID on '_t' to 'c'
                            uint32_t size_dec = 1; // Initialize size decrement
                            for (uint32_t j = 0; j < (t[_t[node.second+3]]&num_c); j++) size_dec += t[_t[node.second+3]+2*j+3]; // Compute size decrement
                            size -= size_dec; // Then decrement the new node size
                            total_number++; // Increment 'total_number'
                            total_size -= size_dec; // And decrement 'total_size'
                        }
                    }
                    new_node = addNodeOn_T(_t, child, size, c); // Create the new node on '_t'
                } else { // If child is already a cover element
                    for (pair<uint32_t,uint32_t> node : c_ref) { // For each node in 'c_ref'
                        if (child == node.first) { // If it corresponds to 'child'
                            new_node = node.second; // Mark it as the new node
                            total_number++; // And increment 'total_number'
                            break; // Then stop searching
                        }
                    }
                }
                s.push(new_node); // Eventually push the root of the new connected component to the stack
            }
            // If necessary, update nodes on '_t' for 'tc's parent
            if (_t[_tc+3] != tc) { // If the centroid is not the root of its cover element
                // Undo '_tc' deletion and update its parameters
                uint32_t parent = _t[_tc+1];
                if (parent != _tc) _t[parent]++; // Get back reference on '_tc's parent
                _t[_tc] -= total_number; // Decrement number of children on '_t'
                _t[_tc+2] -= total_size; // Update size of cover element
                uint32_t i = 0; // Initialize counter
                for (pair<uint32_t,uint32_t> node : c_ref) { // For each node in 'c_ref'
                    if (_t[root+3] == node.first) { // If it was attached "before" than the removed centroid (i.e. 'tc')
                        _t[_tc+3*i+4] = node.second; // Then add its ID among the updated '_tc's children
                        _t[node.second+1] = _tc; // And set '_tc' as its parent
                        i++; // Increment counter
                    }
                }
            }
            if (t[tc+1] != tc) { // If centroid on 't' has a parent
                s.push(root); // And push it to stack
                nc++; // Eventually increment 'nc'
            }
            out[pos] = 0; // Print "("
            out[pos+1] = tc + 2; // Print centroid ID
            pos += 2; // Increment pointer
            if (!noc.empty()) noc.top()--; // Decrement its parent's number of children
            noc.push(nc); // Push its number of chidren to 'noc'
        } else { // When 'dimens' is smaller or equal to 'B'
            vector<uint32_t> tmp = stdCentroidDecomposition(t, _t[root+3], dimens); // Then compute standard centroid decompsition
            for (uint32_t el : tmp) { // And copy resulting vector to 'out'
                out[pos] = el;
                pos++;
            }
            if (!noc.empty()) noc.top()--; // Fix 'noc'
        }
        while (!noc.empty() && noc.top() == 0) { // While there are nodes with no more children
            noc.pop(); // Delete them from 'noc'
            out[pos] = 1; // Print ")"
            pos++; // Increment pointer
        }
    }
    return out; // Return vector rempresentation of the centroid tree
}

/*
 * CORRECTNESS CHECK
 */

// Check correctness of a centroid decomposition
// @param t: tree structure
// @param ctree: string representation of computed centroid tree
// @return true if centroid tree is correct, false otherwise
bool checkCorrectness(vector<uint32_t> &t, const vector<uint32_t> &ct) { // Complexity: unknown and not relevant
    vector<uint32_t> roots; roots.pb(0); // Vector holding all the subtrees' roots
    stack<uint32_t> noc; noc.push(1); // Stack to store number of children of each node
    uint32_t i = 0; // 'ct' vector pointer
    for (uint32_t el : ct) {
        if (el == 0) { // If there is a new node
            uint32_t id = ct[i+1] - 2; // ID of the node
            // Check correctness
            bool found = false; // Initialize 'found' to false
            for (uint32_t j = roots.size()-noc.top(); j < roots.size(); j++) { // For each of the lastly added roots
                uint32_t r = roots[j]; // Current root being analysed
                if (id == stdFindCentroid(t, r)) { // If the centroid of this root match with the build centroid tree
                    found = true; // Mark 'found' as true
                    roots.erase(roots.begin()+j); // Delete this root from 'roots'
                    noc.top()--; // Decrement last node's number of children
                    rmNodeOnT(t, id); // Remove the centroid from 't'
                    uint32_t c = 0; // Initialize new roots counter
                    for(uint32_t k = (t[id]&num_c); k > 0; k--) { // For each of the centroid's children
                        roots.pb(t[id+2*k]); // Push it to the back of 'roots'
                        c++; // And increment counter
                    }
                    if (id != r) { // If the centroid is not the root of the subtree
                        roots.pb(r); // Push the root to 'roots'
                        c++; // And increment counter
                    }
                    noc.push(c); // Eventually push the counter to 'noc'
                    break; // Then stop searching
                }
            }
            if (!found) return false; // If the centroid doesn't match, the whole centroid tree is incorrect
        } else if (el == 1) { // Otherwise, if the current node is closed (i.e. has no more children)
            noc.pop(); // Pop last element from 'noc'
        }
        i++; // Eventually increment pointer
    }
    return true; // If everything was fine, the centroid tree was built correctly
}

/*
 * UTILS
 */

// Get time
// @return timestamp
inline chrono::high_resolution_clock::time_point getTime() { // Complexity: O(1)
    return chrono::high_resolution_clock::now(); // Get new time reference and return
}

// Print elapsed time
// @param t: short description of time count
// @param t1: initial time
// @patam t2: final time
// @return string formatted representation of elapsed time
inline string printTime(const string t, const chrono::high_resolution_clock::time_point t1, const chrono::high_resolution_clock::time_point t2) { // Complexity: O(1)
    auto duration = chrono::duration_cast<chrono::microseconds>(t2-t1).count(); // Compute duration
    int us = duration; // Microseconds
    int ms = us/1000; us %= 1000; // Milliseconds
    int s = ms / 1000; ms %= 1000; // Seconds
    int m = s/60; s %= 60; // Minutes
    int h = m/60; m %= 60; // Hours
    oss os; // New output stream
    os << t << ": " << h << "h " << m << "m " << s << "s " << ms << "ms " << us << "us"; // Print to 'os'
    return os.str(); // Return stream content as a string
}

// Print a vector<uint32_t>
// @param t: tree structure
// @return string representation of 't'
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

// Convert the representation of centroid tree from vector<uint32_t> to string
// @param ct: centroid tree
// @return string representation of centroid tree
inline string ctToString(const vector<uint32_t> &ct) { // Complexity: O(n)
    oss os; // New output stream
    for (uint32_t el : ct) { // For each element in the vector
        switch (el) {
            case 0: // If it is 0
                os << "("; // Then print "("
                break;
            case 1: // If it is 1
                os << ")"; // Then print ")"
                break;
            default: // Otherwise
                os << el - 2; // Print node ID
                break;
        }
    }
    return os.str(); // Return string representation of centroid tree
}

#endif