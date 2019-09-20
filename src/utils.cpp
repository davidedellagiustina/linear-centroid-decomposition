#ifndef UTILS
#define UTILS

#include "main.hpp"
using namespace std;

/*
 * STRUCTURE BUILDING FUNCTIONS
 */

// Build minimal T structure from balanced parenthesis representation [level-wise, in-place]
// @param tree      BP representation of tree
// @return          minimal T representation (no partial sizes)
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
    return t;
}

// Build a reference bitvector to identify the positions of the nodes in T
// @param t     minimal T representation
// @return      nodes reference bitvector
vector<bool> buildIdRef(const vector<uint32_t> &t) { // Complexity: O(n)
    vector<bool> id_ref = vector<bool>(t.size(), 0);
    uint32_t i = 0;
    while (i < t.size()) {
        id_ref[i] = 1;
        i += 2*(t[i]&num_c)+2;
    }
    return id_ref;
}

// Compute the initial partial sizes on T
// @param t         minimal T representation
// @param id_ref    nodes reference bitvector
void computeSizes(vector<uint32_t> &t, const vector<bool> &id_ref) { // Complexity: O(n)
    uint32_t i = t.size() - 2; // ID of the last node
    uint32_t p = t.size(); // Parent (invalid at beginning)
    uint32_t nc = 0; // 'i' is the 'nc'-th child of 'p'
    while (i > 0) {
        if (id_ref[i]) { // If there is a node with this ID on T
            nc = ((t[i+1] != p)? (t[t[i+1]]&num_c)-1 : nc-1);
            p = t[i+1];
            uint32_t size = 1; // Size of subtree rooted at 'i'
            for (uint32_t j = 0; j < (t[i]&num_c); j++) size += t[i+2*j+3];
            t[p+2*nc+3] = size;
        }
        i--;
    }
}

// Cover T and build T2, then compute partial sizes on T
// Note: 'computeSizes()' shouldn't be called: this procedure already computes those sizes
// @param t         minimal T representation
// @param id_ref    nodes reference bitvector
// @param A         minimum size of cover elements - log(n) if not given
// @return          T2 minimal representation (no weights)
vector<uint32_t> cover(vector<uint32_t> &t, const vector<bool> &id_ref, uint32_t A = 0) { // Complexity: O(n)
    uint32_t n = (t.size() + 2) / 4; // Number of nodes of T
    A = ((!A)? ((n <= 1)? 1 : floor(log2(n))) : A); // If A is not given
    uint32_t k = n/A + ((n%A == 0)? 0 : 1) + 1; // Upper-bound for number of nodes of T2
    vector<uint32_t> X = vector<uint32_t>(n);
    vector<tuple<uint32_t,uint32_t,uint32_t,uint32_t>> q = vector<tuple<uint32_t,uint32_t,uint32_t,uint32_t>>(k); // Fields: depth, pre_ord, size, t_node
    uint32_t q_ptr = q.size() - 1; // Pointer on 'q'
    // Step 1 - bottom-up visit [compute partial sizes on T and perform covering]: O(n)
    int64_t i = t.size() - 1, p = t.size(), nc = 0;
    auto x = (uint8_t*)X.data(); // Here we use the first n bytes of 'X'
    uint32_t x1_ptr = 0, x2_ptr = 0; // 'x1_ptr' is at level L, 'x2_ptr' is at level L+1
    while (i >= 0) {
        if (id_ref[i]) {
            nc = ((t[i+1] != p)? (t[t[i+1]]&num_c)-1 : nc-1); p = t[i+1];
            // Compute partial and cover elements sizes
            t[p+2*nc+3] = ((i != 0)? 1 : t[p+2*nc+3]);
            uint32_t size = 1;
            for (uint32_t j = (t[i]&num_c); j > 0; j--, x2_ptr++) {
                t[p+2*nc+3] += ((i != 0)? t[i+2*j+1] : 0);
                size += x[x2_ptr];
            }
            // Create cover element
            if (size >= A || i == 0) {
                t[i] |= cov_el; // Mark node on T as cover element
                std::get<2>(q[q_ptr]) = size; // Write 'size' on 'q'
                std::get<3>(q[q_ptr]) = i; // Write 't_node' on 'q'
                q_ptr--;
                x[x1_ptr] = 0;
            } else x[x1_ptr] = size; // Save cover element size on 'x'
            x1_ptr++;
        }
        i--;
    }
    // Step 2 - top-down visit [compute 'depth' and 'pre-ord' fields for each node on T2]: O(n)
    i = 0; p = 0; q_ptr++; X[0] = 0;
    uint32_t p_depth = 0, pre_ord, X0_ptr = 0, X1_ptr = 0, X2_ptr = 1; // 'X0_ptr' is at level L-1, 'X1_ptr' is at level L and 'X2_ptr' is at level L+1
    while (i < t.size()) {
        k = p + 2*(t[p]&num_c) + 2; // Next node on T (in BFS visit)
        X0_ptr += ((t[i+1] != p)? (t[i+1]-k+2)/2 : 0);
        p_depth = ((t[i+1] != p || t[i+1] == 0)? X[X0_ptr] : p_depth);
        p = t[i+1];
        pre_ord = X[X1_ptr];
        // Compute and save 'pre_ord' field for 'i''s children
        uint32_t s = 1 + pre_ord;
        for (uint32_t j = 0; j < (t[i]&num_c); j++, X2_ptr++) {
            X[X2_ptr] = s;
            s += t[i+2*j+3];
        }
        // Save 'pre_ord' on 'q' for node 'i' if it is marked as cover element
        // Then compute and save 'depth' for node 'i'
        uint32_t z = 0;
        if (t[i]&cov_el) {
            z = 1;
            std::get<1>(q[q_ptr]) = pre_ord; // Save 'pre_ord'
            std::get<0>(q[q_ptr]) = p_depth + z; // Save 'depth'
            q_ptr++;
        }
        X[X1_ptr] = p_depth + z;
        X1_ptr++;
        i += 2*(t[i]&num_c)+2;
    }
    // Step 3 - build minimal T2 [no parent-children pointers]: O(n/log(n))
    std::sort(q.begin(), q.end()); // Sort 'q' lexicographically (first 'depth', then 'pre_ord')
    uint32_t q1_ptr = 0, q2_ptr; while (std::get<2>(q[q1_ptr]) == 0) q1_ptr++; q2_ptr = q1_ptr + 1; // Position 'q1_ptr' at first tuple, 'q2_ptr' at the next one
    uint32_t m = q.size() - q1_ptr; // Number of nodes of T2
    vector<uint32_t> t2 = vector<uint32_t>(7*m-3);
    i = 0;
    while (i < t2.size()) {
        t2[i+2] = std::get<2>(q[q1_ptr]); // Size of subtree
        t2[i+3] = std::get<3>(q[q1_ptr]); // ID reference on T
        i += 4; nc = 0;
        if (q1_ptr+1 < q.size()) { // If this isn't the last tuple
            if (std::get<0>(q[q1_ptr+1]) > std::get<0>(q[q1_ptr])) { // If next node is at level L+1, then all nodes at L+1, starting from 'q2_ptr', are 'i''s children
                uint32_t l = std::get<0>(q[q1_ptr]) + 1; // Level L+1
                while (q2_ptr < q.size() && std::get<0>(q[q2_ptr]) == l) {
                    nc++; q2_ptr++;
                    i += 3;
                }
            } else { // Otherwise
                uint32_t l = std::get<0>(q[q1_ptr]) + 1; // Level L+1
                while (q2_ptr < q.size() && std::get<0>(q[q2_ptr]) == l && std::get<1>(q[q2_ptr]) < std::get<1>(q[q1_ptr+1])) { // Count 'i''s children at L+1 using 'pre_ord'
                    nc++; q2_ptr++;
                    i += 3;
                }
            }
        }
        t2[i-3*nc-4] = nc; // Write number of children
        q1_ptr++;
    }
    // Step 4 - compute parent-children pointers: O(n/log(n))
    i = 0;
    uint32_t j = 3*t2[i]+4; // Second node in BFS
    while (i < t2.size()) {
        for (uint32_t k = 0; k < t2[i]; k++) {
            t2[i+3*k+4] = j; t2[j+1] = i;
            j += 3*t2[j]+4; // Next child
        }
        i += 3*t2[i]+4; // Next node
    }
    return t2;
}

/*
 * STANDARD O(n*log(n)) CENTROID DECOMPOSITION IMPLEMENTATION
 */

// Remove a node 'n' from T
// @param t     T representation
// @param n     ID of the note to be removed
inline void rmNodeOnT(vector<uint32_t> &t, const uint32_t n) { // Complexity: O(k) where k = t[t[n+1]]
    uint32_t p = t[n+1];
    uint32_t size; for (uint32_t i = 0; i < (t[p]&num_c); i++) if (t[p+2*i+2] == n) size = t[p+2*i+3]; // Size of 'n'
    // Delete references inside 'n''s parent
    if (p != n) { // If 'n' has a parent
        for (uint32_t i = 0; i < (t[p]&num_c); i++) { // Search among its 'p''s children
            if (t[p+2*i+2] == n) { // Node found
                // Swap its ID and size with the last valid child's ID and size
                uint32_t aux_id = t[p+2*i+2], aux_size = t[p+2*i+3];
                t[p+2*i+2] = t[p+2*(t[p]&num_c)]; t[p+2*i+3] = t[p+2*(t[p]&num_c)+1];
                t[p+2*(t[p]&num_c)] = aux_id; t[p+2*(t[p]&num_c)+1] = aux_size;
                break; // Stop searching
            }
        }
        t[p]--; // Decrement 'p''s number of children
        // Update partial sizes on T
        uint32_t m = p; p = t[m+1]; // Starting from 'p'
        while (m != p) { // Navigate up the tree
            for (uint32_t i = 0; i < (t[p]&num_c); i++) if (t[p+2*i+2] == m) t[p+2*i+3] -= size;
            m = p; p = t[m+1]; // Step up
        }
    }
    // Delete references inside 'n''s children
    for (uint32_t i = 0; i < (t[n]&num_c); i++) t[t[n+2*i+2]+1] = t[n+2*i+2];
}

// Add a node on T2
// Note: when this function is called, the newly added node is ALWAYS the root of a connected component. Therefore, we assume this condition to be true.
// @param t2            T2 representation
// @param ref           reference ID on T
// @param size          size of treelet
// @param children      vector of the children of the new node
// @return              ID of the newly added node
inline uint32_t addNodeOnT2(vector<uint32_t> &t2, const uint32_t ref, const uint32_t size, const vector<uint32_t> &children) { // Complexity: O(k) where k = children.size()
    uint32_t id = t2.size(); // ID of the new node
    t2.pb(children.size()); // Number of children
    t2.pb(id); // Parent ID (i.e. itself, see assumption above)
    t2.pb(size); // Size of treelet
    t2.pb(ref); // Treelet root ID reference on T
    for (uint32_t child : children) {
        t2.pb(child); // Child ID
        t2.pb(0); t2.pb(0); // Empty deltas (will be computed by the proper function)
        t2[child+1] = id; // Update child's parent ID
    }
    return id;
}

// Remove a node 'n' from T2
// @param t2    T2 representation
// @param n     ID of the node to be removed
// @return      vector with the children of the removed node
inline vector<uint32_t> rmNodeOnT2(vector<uint32_t> &t2, const uint32_t n) { // Complexity: O(k) where k = _t[_t[n+1]]
    uint32_t p = t2[n+1]; // Parent of the node ID
    // Delete references inside 'n''s parent
    if (p != n) {
        for (uint32_t i = 0; i < t2[p]; i++) { // Search among 'p' children
            if (t2[p+3*i+4] == n) { // Node found
                // Swap its ID and deltas with the last valid child's ID and deltas
                uint32_t aux_id = t2[p+3*i+4], aux_delta_1 = t2[p+3*i+5], aux_delta_2 = t2[p+3*i+6];
                t2[p+3*i+4] = t2[p+3*t2[p]+1]; t2[p+3*i+5] = t2[p+3*t2[p]+2]; t2[p+3*i+6] = t2[p+3*t2[p]+3];
                t2[p+3*t2[p]+1] = aux_id; t2[p+3*t2[p]+2] = aux_delta_1; t2[p+3*t2[p]+3] = aux_delta_2;
                break; // Stop searching
            }
        }
        t2[p]--; // Decrement 'p''s number of children
    }
    // Delete references inside 'n''s children
    vector<uint32_t> children;
    for (uint32_t i = 0; i < t2[n]; i++) { // Navigate the children
        t2[t2[n+3*i+4]+1] = t2[n+3*i+4]; // And make them new roots of subtrees
        children.pb(t2[n+3*i+4]);
    }
    return children;
}

// Standard centroid search algorithm
// @param t         T representation
// @param root      root of the connected component
// @return          centroid of the connected component
inline uint32_t stdFindCentroid(const vector<uint32_t> &t, const uint32_t root) { // Complexity: O(n)
    // Compute half size of subtree: O(k) where k = (t[root]&num_c)
    uint32_t half_size = 1; for (uint32_t i = 0; i < (t[root]&num_c); i++) half_size += t[root+2*i+3];
    half_size /= 2;
    // Centroid search: O(n)
    uint32_t centroid = root; // Start search from root
    bool found = false;
    if ((t[centroid]&num_c) > 0) {
        while (!found) {
            for (uint32_t i = 0 ; i < (t[centroid]&num_c); i++) {
                if (t[centroid+2*i+3] > half_size) { // Look for heavy child
                    centroid = t[centroid+2*i+2];
                    found = false; break;
                } else found = true; // Centroid found
            }
        }
    }
    return centroid;
}

// Standard centroid decomposition algorithm
// @param t         T representation
// @param root      root of the tree (or connected component, used as subprocedure for linear centroid decomposition)
// @param N         number of nodes of the tree to elaborate (require ONLY when called as subprocedure of linear centroid decomposition)
// @return          vector representation of the centroid tree
inline vector<uint32_t> stdCentroidDecomposition(vector<uint32_t> &t, const uint32_t root = 0, uint32_t N = 0) { // Complexity: O(n*log(n))
    N = ((!N)? (t.size()+2)/4 : N);
    vector<uint32_t> out = vector<uint32_t>(3*N, 0); uint32_t ptr = 0;
    stack<uint32_t> s; s.push(root); // Stack with roots of connected components yet to process
    while (!s.empty()) {
        uint32_t r = s.top(); s.pop();
        uint32_t centroid = stdFindCentroid(t, r);
        uint32_t c = 0; for (uint32_t i = 0; i < (t[centroid]&num_c); i++) c += t[centroid+2*i+3]; // Total size of centroid subtrees [needed for printing centroid tree]
        rmNodeOnT(t, centroid);
        for(uint32_t i = (t[centroid]&num_c); i > 0; i--) s.push(t[centroid+2*i]); // Push children to stack in reverse order
        if (centroid != r) { // If the root of the subtree is not its centroid
            s.push(r);
            c++; for (uint32_t i = 0; i < (t[r]&num_c); i++) c += t[r+2*i+3]; // Size of root subtree [needed for printing centroid tree]
        }
        // Print the current node to the output vector
        while (out[ptr] == 1) ptr++;
        out[ptr] = 0; // Print "("
        out[ptr+1] = centroid + 2; // Print centroid ID
        ptr += 2;
        out[ptr+3*c] = 1; // Print ")"
    }
    return out;
}

/*
 * NEW O(n) CENTROID DECOMPOSITION IMPLEMENTATION
 */

// Recompute the deltas on a connected component of T2
// @param t         T representation
// @param t2        T2 representation
// @param root      root of the connected component of which to compute deltas
inline void computeDeltas(const vector<uint32_t> &t, vector<uint32_t> &t2, const uint32_t root) { // Complexity: O(log(n))
    // Compute total size of treelet
    uint32_t size = 1; for (uint32_t i = 0; i < (t[t2[root+3]]&num_c); i++) size += t[t2[root+3]+2*i+3];
    // Build stack for DFS
    stack<uint32_t> s, dfs; s.push(root);
    while (!s.empty()) {
        uint32_t node = s.top(); s.pop();
        dfs.push(node);
        for (uint32_t i = t2[node]; i > 0; i--) s.push(t2[node+3*i+1]); // Push children
    }
    // DFS on T2 and compute deltas
    while (!dfs.empty()) {
        uint32_t node = dfs.top(); dfs.pop();
        for (uint32_t i = 0; i < t2[node]; i++) {
            t2[node+3*i+5] = t2[t2[node+3*i+4]+2]; for (uint32_t j = 0; j < t2[t2[node+3*i+4]]; j++) t2[node+3*i+5] += t2[t2[node+3*i+4]+3*j+5]; // Delta 1
            t2[node+3*i+6] = size - t2[node+3*i+5]; // Delta 2
        }
    }
}

// New centroid search algorithm
// @param t         T representation
// @param t2        T2 representation
// @param root      root of the connected component
// @return          centroid of the connected component (both IDs on T and T2)
inline pair<uint32_t,uint32_t> findCentroid(const vector<uint32_t> &t, const vector<uint32_t> &t2, const uint32_t root) { // Complexity: O(n/log(n))
    // Compute half size of connected component: O(1)
    uint32_t half_size = ((t2[root] == 0)? t2[root+2] : t2[root+5]+t2[root+6]);
    half_size /= 2; // Compute size
    // Serach centroid treelet on T2: O(n/log(n))
    uint32_t centroid_treelet = root; // Start searching from root
    bool found = false;
    while (!found) {
        if (t2[centroid_treelet] > 0) {
            for (uint32_t i = 0; i < t2[centroid_treelet]; i++) { // Search heavy child
                if (t2[centroid_treelet+3*i+5] > half_size) {
                    centroid_treelet = t2[centroid_treelet+3*i+4];
                    found = false; break;
                } else found = true; // Centroid treelet found
            }
        } else found = true;
    }
    // Search centroid node on T [visit subtree]: O(log(n))
    uint32_t centroid_node = t2[centroid_treelet+3];
    found = false;
    if ((t[centroid_node]&num_c) > 0) {
        while (!found) {
            for (uint32_t i = 0; i < (t[centroid_node]&num_c); i++) { // Search heavy child
                if (t[centroid_node+2*i+3] > half_size) {
                    centroid_node = t[centroid_node+2*i+2];
                    found = false; break;
                } else found = true;
            }
        }
    }
    return make_pair(centroid_treelet, centroid_node);
}

// New centroid decomposition algorithm
// @param t: tree structure
// @param _t: '_t' tree structure
// @param B: maximum size when to apply standard centroid decomposition - (log(n))^3 if not given
// @return vector representation of the centroid tree
// vector<uint32_t> centroidDecomposition(vector<uint32_t> &t, vector<uint32_t> &_t, uint32_t B = 0) { // Complexity: O(n)
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
            computeDeltas(t, _t, root); // Compute deltas of connected component
            pair<uint32_t,uint32_t> centroid = findCentroid(t, _t, root); // Find centroid of subtree with root 'root'
            uint32_t _tc = centroid.first, tc = centroid.second; // Parse centroid nodes on 't' and '_t'
            rmNodeOnT(t, tc); // Remove node 'tc' from 't'
            vector<uint32_t> children = rmNodeOnT2(_t, _tc); // Remove node '_tc' from '_t' and get its children on '_t'
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
                    new_node = addNodeOnT2(_t, child, size, c); // Create the new node on '_t'
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
// @return      timestamp
inline chrono::high_resolution_clock::time_point getTime() { // Complexity: O(1)
    return chrono::high_resolution_clock::now();
}

// Print elapsed time
// @param t     short description of time count
// @param t01   initial time
// @patam t02   final time
// @return      formatted string representation of elapsed time
inline string printTime(const string t, const chrono::high_resolution_clock::time_point t01, const chrono::high_resolution_clock::time_point t02) { // Complexity: O(1)
    auto duration = chrono::duration_cast<chrono::microseconds>(t02-t01).count(); // Compute duration
    int us = duration; // Microseconds
    int ms = us/1000; us %= 1000; // Milliseconds
    int s = ms / 1000; ms %= 1000; // Seconds
    int m = s/60; s %= 60; // Minutes
    int h = m/60; m %= 60; // Hours
    oss os; os << t << ": " << h << "h " << m << "m " << s << "s " << ms << "ms " << us << "us"; // Print to output stream
    return os.str();
}

// Print a vector<uint32_t> [used for debugging purposes]
// @param t     general tree representation
// @return      string representation of tree
inline string print(const vector<uint32_t> &t) { // Complexity: O(n)
    oss os;
    os << "(";
    bool first = true; // Used for the first element
    for (const uint32_t i : t) {
        if (first) {
            os << i;
            first = false;
        } else os << " " << i;
    }
    os << ")";
    return os.str();
}

// Convert the representation of centroid tree from vector<uint32_t> to string
// @param ct    centroid tree vector representation
// @return      centroid tree string representation
inline string ctToString(const vector<uint32_t> &ct) { // Complexity: O(n)
    oss os;
    for (uint32_t el : ct) {
        switch (el) {
            case 0: os << "("; break;
            case 1: os << ")"; break;
            default: os << el-2; break;
        }
    }
    return os.str();
}

#endif