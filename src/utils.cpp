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
    uint32_t n = tree.length() / 2;
    uint32_t N = (4 * n) - 2; // Size of T
    vector<uint32_t> t = vector<uint32_t>(N); // Empty T
    uint32_t H = 0; // Max height
    // Compute the number of nodes per level in T[0...H-1]
    int64_t h = 1; // Current height
    for (uint32_t i = 0; i < tree.length(); ++i) {
        t[h-1] += (tree[i] == '(');
        h += ((tree[i] == '(')? 1 : -1);
        H = std::max(uint32_t(h), H); // Also compute max height
    }
    // Compute the partial sums in T[0...H-1]
    uint32_t psum = 0, tmp;
    for (uint32_t i = 0; i < H; ++i) {
        tmp = t[i];
        t[i] = psum;
        psum += tmp;
    }
    // Count the number of children for each node. We store this info in T[H...H+n-1]
    h = 0;
    for (uint32_t i = 1; i < tree.length(); ++i) {
        t[H+t[h]] += (tree[i] == '(');
        if (t[H+t[h]] > max_deg) throw "Tree is too big: out-degree overflow."; // Check for out-degree overflow
        t[h] += (tree[i] == ')');
        h += ((tree[i] == '(')? 1 : -1);
    }
    for (uint32_t i = 0; i < n; ++i) t[i] = t[i+H]; // Left-shift T[H...H+n-1] to T[0...n-1]
    // Build basic T representation (no parent-children pointers)
    uint32_t r = n; // T[r] is the number of children of last allocated node
    uint32_t R = N; // Start position (=ID) in T of last allocated node
    for(uint32_t i = 0; i < n; ++i) {
        --r; // Process next node (right to left)
        R -= (2 * t[r]) + 2; // Node ID
        t[R] = t[r]; // Number of children
    }
    // Set parent-children pointers
    uint32_t i = 0, j = 2*t[0] + 2; // 'i' is at level L, 'j' is at level L+1
    t[1] = 0; // Parent of root = root
    while (j < N) {
        uint32_t x = i; // This node
        uint32_t nc = t[x]; // Number of children of this node
        i += 2; // Jump to children area
        for (int k = 0; k < nc; ++k) {
            t[i] = j; // Write child's ID
            i += 2; // Jump to next child 
            t[j+1] = x; // Store parent of 'j'
            j += 2*t[j]+2; // Jump to next child's area
        }
    }
    return t;
}

// Build a reference vector to identify the positions of the nodes in T
// @param t     minimal T representation
// @return      nodes reference vector
vector<uint32_t> buildIdRef(const vector<uint32_t> &t) { // Complexity: O(n)
    vector<uint32_t> id_ref = vector<uint32_t>(sizeOfT(t));
    uint32_t i = 0, j = 0;
    while (i < t.size()) {
        id_ref[j] = i;
        i += 2*(t[i]&num_c)+2; ++j; // Next node
    }
    return id_ref;
}

// Compute the initial partial sizes on T
// @param t         minimal T representation
// @param id_ref    nodes reference vector
void computeSizes(vector<uint32_t> &t, const vector<uint32_t> &id_ref) { // Complexity: O(n)
    uint32_t p = t.size(); // Parent (invalid at beginning)
    uint32_t nc = 0; // 'i' is the 'nc'-th child of 'p'
    for (auto it = id_ref.rbegin(); it != id_ref.rend()-1; ++it) {
        uint32_t i = *it; // ID of node
        nc = ((t[parnt(i)] != p)? (t[t[parnt(i)]]&num_c)-1 : nc-1);
        p = t[parnt(i)];
        uint32_t size = 1; for (uint32_t j = 0; j < (t[i]&num_c); ++j) size += t[sizeOfChildOnT(i, j)]; // Size of subtree rooted at 'i'
        t[sizeOfChildOnT(p, nc)] = size;
    }
}

// Cover T and build T2, then compute partial sizes on T
// Note: 'computeSizes()' shouldn't be called: this procedure already computes those sizes
// @param t         minimal T representation
// @param id_ref    nodes reference vector
// @param A         minimum size of cover elements - log(n) if not given
// @return          T2 minimal representation (no weights)
vector<uint32_t> cover(vector<uint32_t> &t, const vector<uint32_t> &id_ref, uint32_t A = 0) { // Complexity: O(n)
    uint32_t n = sizeOfT(t); // Number of nodes of T
    A = ((!A)? ((n <= 1)? 1 : log2(n)) : A); // If A is not given
    if (A > max_A) throw "\"A\" parameter is too big: maximum is 65535.";
    uint32_t k = n/A + ((n%A == 0)? 0 : 1) + 1; // Upper-bound for number of nodes of T2
    vector<uint32_t> X = vector<uint32_t>(n);
    vector<tuple<uint32_t,uint32_t,uint32_t,uint32_t>> q = vector<tuple<uint32_t,uint32_t,uint32_t,uint32_t>>(k); // Fields: depth, pre_ord, size, t_node
    uint32_t q_ptr = q.size() - 1; // Pointer on 'q'
    // Step 1 - bottom-up visit [compute partial sizes on T and perform covering]: O(n)
    int64_t i, p = t.size(), nc = 0;
    auto x = (uint16_t*)X.data(); // Here we use the first n bytes of 'X'
    uint32_t x1_ptr = 0, x2_ptr = 0; // 'x1_ptr' is at level L, 'x2_ptr' is at level L+1
    for (auto it = id_ref.rbegin(); it != id_ref.rend(); ++it) {
        i = *it; nc = ((t[parnt(i)] != p)? (t[t[parnt(i)]]&num_c)-1 : nc-1); p = t[parnt(i)];
        // Compute partial and cover elements sizes
        t[sizeOfChildOnT(p, nc)] = ((i != 0)? 1 : t[sizeOfChildOnT(p, nc)]);
        uint32_t size = 1;
        for (uint32_t j = (t[i]&num_c); j > 0; --j, ++x2_ptr) {
            t[sizeOfChildOnT(p, nc)] += ((i != 0)? t[sizeOfChildOnT(i, j-1)] : 0);
            size += x[x2_ptr];
        }
        // Create cover element
        if (size >= A || i == 0) {
            t[i] |= cov_el; // Mark node on T as cover element
            std::get<2>(q[q_ptr]) = size; // Write 'size' on 'q'
            std::get<3>(q[q_ptr]) = i; // Write 't_node' on 'q'
            --q_ptr;
            x[x1_ptr] = 0;
        } else x[x1_ptr] = size; // Save cover element size on 'x'
        ++x1_ptr;
    }
    // Step 2 - top-down visit [compute 'depth' and 'pre-ord' fields for each node on T2]: O(n)
    i = 0; p = 0; ++q_ptr; X[0] = 0;
    uint32_t p_depth = 0, pre_ord, X0_ptr = 0, X1_ptr = 0, X2_ptr = 1; // 'X0_ptr' is at level L-1, 'X1_ptr' is at level L and 'X2_ptr' is at level L+1
    while (i < t.size()) {
        k = p + 2*(t[p]&num_c) + 2; // Next node on T (in BFS visit)
        X0_ptr += ((t[parnt(i)] != p)? (t[parnt(i)]-k+2)/2 : 0);
        p_depth = ((t[parnt(i)] != p || t[parnt(i)] == 0)? X[X0_ptr] : p_depth);
        p = t[parnt(i)];
        pre_ord = X[X1_ptr];
        // Compute and save 'pre_ord' field for 'i''s children
        uint32_t s = 1 + pre_ord;
        for (uint32_t j = 0; j < (t[i]&num_c); ++j, ++X2_ptr) {
            X[X2_ptr] = s;
            s += t[sizeOfChildOnT(i, j)];
        }
        // Save 'pre_ord' on 'q' for node 'i' if it is marked as cover element
        // Then compute and save 'depth' for node 'i'
        uint32_t z = 0;
        if (t[i]&cov_el) {
            z = 1;
            std::get<1>(q[q_ptr]) = pre_ord; // Save 'pre_ord'
            std::get<0>(q[q_ptr]) = p_depth + z; // Save 'depth'
            ++q_ptr;
        }
        X[X1_ptr] = p_depth + z;
        ++X1_ptr;
        i += 2*(t[i]&num_c)+2;
    }
    // Step 3 - build minimal T2 [no parent-children pointers]: O(n/log(n))
    std::sort(q.begin(), q.end()); // Sort 'q' lexicographically (first 'depth', then 'pre_ord')
    uint32_t q1_ptr = 0, q2_ptr; while (std::get<2>(q[q1_ptr]) == 0) ++q1_ptr; q2_ptr = q1_ptr + 1; // Position 'q1_ptr' at first tuple, 'q2_ptr' at the next one
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
                    ++nc; ++q2_ptr;
                    i += 3;
                }
            } else { // Otherwise
                uint32_t l = std::get<0>(q[q1_ptr]) + 1; // Level L+1
                while (q2_ptr < q.size() && std::get<0>(q[q2_ptr]) == l && std::get<1>(q[q2_ptr]) < std::get<1>(q[q1_ptr+1])) { // Count 'i''s children at L+1 using 'pre_ord'
                    ++nc; ++q2_ptr;
                    i += 3;
                }
            }
        }
        t2[i-3*nc-4] = nc; // Write number of children
        ++q1_ptr;
    }
    // Step 4 - compute parent-children pointers: O(n/log(n))
    i = 0;
    uint32_t j = 3*t2[i]+4; // Second node in BFS
    while (i < t2.size()) {
        for (uint32_t k = 0; k < t2[i]; ++k) {
            t2[childOnT2(i, k)] = j; t2[parnt(j)] = i;
            j += childOnT2(0, t2[j]); // Next child
        }
        i += childOnT2(0, t2[i]); // Next node
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
    uint32_t p = t[parnt(n)];
    uint32_t size; for (uint32_t i = 0; i < (t[p]&num_c); ++i) if (t[childOnT(p, i)] == n) size = t[sizeOfChildOnT(p, i)]; // Size of 'n'
    // Delete references inside 'n''s parent
    if (p != n) { // If 'n' has a parent
        for (uint32_t i = 0; i < (t[p]&num_c); ++i) { // Search among its 'p''s children
            if (t[childOnT(p, i)] == n) { // Node found
                // Swap its ID and size with the last valid child's ID and size
                uint32_t aux_id = t[childOnT(p, i)], aux_size = t[sizeOfChildOnT(p, i)];
                t[childOnT(p, i)] = t[childOnT(p, (t[p]&num_c)-1)]; t[sizeOfChildOnT(p, i)] = t[sizeOfChildOnT(p, (t[p]&num_c)-1)];
                t[childOnT(p, (t[p]&num_c)-1)] = aux_id; t[sizeOfChildOnT(p, (t[p]&num_c)-1)] = aux_size;
                break; // Stop searching
            }
        }
        --t[p]; // Decrement 'p''s number of children
        // Update partial sizes on T
        uint32_t m = p; p = t[parnt(m)]; // Starting from 'p'
        while (m != p) { // Navigate up the tree
            for (uint32_t i = 0; i < (t[p]&num_c); ++i) if (t[childOnT(p, i)] == m) t[sizeOfChildOnT(p, i)] -= size;
            m = p; p = t[parnt(m)]; // Step up
        }
    }
    // Delete references inside 'n''s children
    for (uint32_t i = 0; i < (t[n]&num_c); ++i) t[parnt(t[childOnT(n, i)])] = t[childOnT(n, i)];
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
    uint32_t p = t2[parnt(n)]; // Parent of the node ID
    // Delete references inside 'n''s parent
    if (p != n) {
        for (uint32_t i = 0; i < t2[p]; ++i) { // Search among 'p' children
            if (t2[childOnT2(p, i)] == n) { // Node found
                // Swap its ID and deltas with the last valid child's ID and deltas
                uint32_t aux_id = t2[childOnT2(p, i)], aux_delta_1 = t2[delta1OfChildOnT2(p, i)], aux_delta_2 = t2[delta2OfChildOnT2(p, i)];
                t2[childOnT2(p, i)] = t2[childOnT2(p, t2[p]-1)]; t2[delta1OfChildOnT2(p, i)] = t2[delta1OfChildOnT2(p, t2[p]-1)]; t2[delta2OfChildOnT2(p, i)] = t2[delta2OfChildOnT2(p, t2[p]-1)];
                t2[childOnT2(p, t2[p]-1)] = aux_id; t2[delta1OfChildOnT2(p, t2[p]-1)] = aux_delta_1; t2[delta2OfChildOnT2(p, t2[p]-1)] = aux_delta_2;
                break; // Stop searching
            }
        }
        --t2[p]; // Decrement 'p''s number of children
    }
    // Delete references inside 'n''s children
    vector<uint32_t> children;
    for (uint32_t i = 0; i < t2[n]; ++i) { // Navigate the children
        t2[parnt(t2[childOnT2(n, i)])] = t2[childOnT2(n, i)]; // And make them new roots of subtrees
        children.pb(t2[childOnT2(n, i)]);
    }
    return children;
}

// Standard centroid search algorithm
// @param t         T representation
// @param root      root of the connected component
// @return          centroid of the connected component
inline uint32_t stdFindCentroid(const vector<uint32_t> &t, const uint32_t root) { // Complexity: O(n)
    // Compute half size of subtree: O(k) where k = (t[root]&num_c)
    uint32_t half_size = 1; for (uint32_t i = 0; i < (t[root]&num_c); ++i) half_size += t[sizeOfChildOnT(root, i)];
    half_size /= 2;
    // Centroid search: O(n)
    uint32_t centroid = root; // Start search from root
    bool found = false;
    if ((t[centroid]&num_c) > 0) {
        while (!found) {
            for (uint32_t i = 0 ; i < (t[centroid]&num_c); ++i) {
                if (t[sizeOfChildOnT(centroid, i)] > half_size) { // Look for heavy child
                    centroid = t[childOnT(centroid, i)];
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
// @param N         number of nodes of the tree to elaborate (required ONLY when called as subprocedure of linear centroid decomposition)
// @return          pair<shape,ids> (struct) representation of the centroid tree
inline struct c_tree stdCentroidDecomposition(vector<uint32_t> &t, const uint32_t root = 0, uint32_t N = 0) { // Complexity: O(n*log(n))
    N = ((!N)? sizeOfT(t) : N);
    struct c_tree ct;
    ct.shape = vector<uint8_t>(2*N, 0);
    ct.ids = vector<uint32_t>(N, 0);
    uint32_t ptr1 = 0, ptr2 = 0; // 'ptr1' for 'shape', 'ptr2' for 'ids'
    // struct stk s; s.init(N); s.push(root); // Stack with roots of connected components yet to process
    stack<int> s; s.push(root);
    while (!s.empty()) {
        uint32_t r = s.top(); s.pop();
        uint32_t c = 0; for (uint32_t i = 0; i < (t[r]&num_c); ++i) c+= t[sizeOfChildOnT(r, i)]; // Total size of future connected components [used for printing output]
        uint32_t centroid = stdFindCentroid(t, r);
        rmNodeOnT(t, centroid);
        for(uint32_t i = (t[centroid]&num_c); i > 0; --i) s.push(t[childOnT(centroid, i-1)]); // Push children to stack in reverse order
        if (centroid != r) s.push(r); // If the root of the subtree is not its centroid, then push it
        // Print the current node to the output structure
        while (ct.shape[ptr1] == 1) ++ptr1;
        ct.shape[ptr1] = 0; // Print "("
        ct.ids[ptr2] = centroid; // Print centroid ID
        ++ptr1; ++ptr2;
        ct.shape[ptr1+2*c] = 1; // Print ")"
    }
    return ct;
}

// Standard centroid decomposition algorithm (with global stack)
// @param s         global custom stack (in order to avoid reallocations of memory)
// @param t         T representation
// @param root      root of the tree (or connected component, used as subprocedure for linear centroid decomposition)
// @param N         number of nodes of the tree to elaborate (required ONLY when called as subprocedure of linear centroid decomposition)
// @return          pair<shape,ids> (struct) representation of the centroid tree
inline struct c_tree stdCentroidDecomposition(struct stk &s, vector<uint32_t> &t, const uint32_t root = 0, uint32_t N = 0) { // Complexity: O(n*log(n))
    N = ((!N)? sizeOfT(t) : N);
    struct c_tree ct;
    ct.shape = vector<uint8_t>(2*N, 0);
    ct.ids = vector<uint32_t>(N, 0);
    uint32_t ptr1 = 0, ptr2 = 0; // 'ptr1' for 'shape', 'ptr2' for 'ids'
    s.push(root);
    while (!s.empty()) {
        uint32_t r = s.top(); s.pop();
        uint32_t c = 0; for (uint32_t i = 0; i < (t[r]&num_c); ++i) c+= t[sizeOfChildOnT(r, i)]; // Total size of future connected components [used for printing output]
        uint32_t centroid = stdFindCentroid(t, r);
        rmNodeOnT(t, centroid);
        for(uint32_t i = (t[centroid]&num_c); i > 0; --i) s.push(t[childOnT(centroid, i-1)]);
        if (centroid != r) s.push(r);
        // Print the current node to the output structure
        while (ct.shape[ptr1] == 1) ++ptr1;
        ct.shape[ptr1] = 0; // Print "("
        ct.ids[ptr2] = centroid; // Print centroid ID
        ++ptr1; ++ptr2;
        ct.shape[ptr1+2*c] = 1; // Print ")"
    }
    return ct;
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
    uint32_t size = 1; for (uint32_t i = 0; i < (t[t2[alpha(root)]]&num_c); ++i) size += t[sizeOfChildOnT(t2[alpha(root)], i)];
    // Build stack for DFS
    // struct stk s; s.init(size); s.push(root);
    stack<int> s; s.push(root);
    // struct stk dfs; dfs.init(size);
    stack<int> dfs;
    while (!s.empty()) {
        uint32_t node = s.top(); s.pop();
        dfs.push(node);
        for (uint32_t i = t2[node]; i > 0; --i) s.push(t2[childOnT2(node, i-1)]); // Push children
    }
    // DFS on T2 and compute deltas
    while (!dfs.empty()) {
        uint32_t node = dfs.top(); dfs.pop();
        for (uint32_t i = 0; i < t2[node]; ++i) {
            t2[delta1OfChildOnT2(node, i)] = t2[t2[childOnT2(node, i)]+2]; for (uint32_t j = 0; j < t2[t2[childOnT2(node, i)]]; ++j) t2[delta1OfChildOnT2(node, i)] += t2[delta1OfChildOnT2(t2[childOnT2(node, i)], j)]; // Delta 1
            t2[delta2OfChildOnT2(node, i)] = size - t2[delta1OfChildOnT2(node, i)]; // Delta 2
        }
    }
}

// New centroid search algorithm
// @param t         T representation
// @param t2        T2 representation
// @param root      root of the connected component
// @return          centroid of the connected component (both IDs on T and T2)
inline pair<uint32_t,uint32_t> findCentroid(const vector<uint32_t> &t, const vector<uint32_t> &t2, const uint32_t root) { // Complexity: O(n/log(n)+log(n))
    // Compute half size of connected component: O(1)
    uint32_t half_size = ((t2[root] == 0)? t2[root+2] : t2[delta1OfChildOnT2(root, 0)]+t2[delta2OfChildOnT2(root, 0)]);
    half_size /= 2; // Compute size
    // Serach centroid treelet on T2: O(n/log(n))
    uint32_t centroid_treelet = root; // Start searching from root
    bool found = false;
    while (!found) {
        if (t2[centroid_treelet] > 0) {
            for (uint32_t i = 0; i < t2[centroid_treelet]; ++i) { // Search heavy child
                if (t2[delta1OfChildOnT2(centroid_treelet, i)] > half_size) {
                    centroid_treelet = t2[childOnT2(centroid_treelet, i)];
                    found = false; break;
                } else found = true; // Centroid treelet found
            }
        } else found = true;
    }
    // Search centroid node on T [visit subtree]: O(log(n))
    uint32_t centroid_node = t2[alpha(centroid_treelet)];
    found = false;
    if ((t[centroid_node]&num_c) > 0) {
        while (!found) {
            for (uint32_t i = 0; i < (t[centroid_node]&num_c); ++i) { // Search heavy child
                if (t[sizeOfChildOnT(centroid_node, i)] > half_size) {
                    centroid_node = t[childOnT(centroid_node, i)];
                    found = false; break;
                } else found = true;
            }
        }
    }
    return make_pair(centroid_treelet, centroid_node);
}

// New centroid decomposition algorithm
// @param t         T representation
// @param t2        T2 representation
// @param B         threshold for standard centroid decomposition - (log(n))^3 if not given
// @return          centroid tree pair<shape,ids> (struct) representation
struct c_tree centroidDecomposition(vector<uint32_t> &t, vector<uint32_t> &t2, uint32_t B = 0) { // Complexity: O(n)
    uint32_t n = (t.size() + 2) / 4;
    B = ((n <= 1)? 1 : ((!B)? (log2(n)*log2(n)*log2(n)) : B));
    struct c_tree ct;
    ct.shape = vector<uint8_t>(2*n, 0);
    ct.ids = vector<uint32_t>(n, 0);
    uint32_t ptr1 = 0, ptr2 = 0;
    // struct stk s; s.init(n); s.push(0); // Stack with roots of connected components yet to process
    stack<int> s; s.push(0);
    struct stk aux_s; aux_s.init(B); // Global auxiliary stack for standard centroid decomposition
    while (!s.empty()) {
        uint32_t r = s.top(); s.pop();
        uint32_t size = 1; for (uint32_t i = 0; i < (t[t2[alpha(r)]]&num_c); ++i) size += t[sizeOfChildOnT(t2[alpha(r)], i)]; // Size of connected component
        if (size > B) { // If connected component is bigger than threshold 'B'
            computeDeltas(t, t2, r);
            pair<uint32_t,uint32_t> centroid = findCentroid(t, t2, r); uint32_t t2c = centroid.first, tc = centroid.second; // Centroid on T and T2
            rmNodeOnT(t, tc);
            vector<uint32_t> children = rmNodeOnT2(t2, t2c);
            // Build children reference vector
            vector<pair<uint32_t,uint32_t>> c_ref;
            for (uint32_t child : children) {
                uint32_t n = t2[alpha(child)], p = t[parnt(n)];
                while (p != n) { n = p; p = t[parnt(n)]; }
                c_ref.pb(make_pair(n, child));
            }
            // Build new nodes on T2 for each 'tc''s children
            uint32_t nc = (t[tc]&num_c); // Number of children [used for printing centroid tree]
            uint32_t total_number = 0; // Total number of old T2 nodes whose parent has been found [used when updating 'tc''s parent]
            uint32_t total_size = 1; // Total size of the newly created nodes on T2 [used when updating 'tc''s parent]
            for (uint32_t i = nc; i > 0; --i) {
                uint32_t new_node;
                uint32_t child = t[childOnT(tc, i-1)];
                if (!(t[child]&cov_el)) { // If 'child' isn't a cover element
                    t[child] |= cov_el;
                    uint32_t size = t[sizeOfChildOnT(tc, i-1)]; total_size += size;
                    vector<uint32_t> c;
                    for (pair<uint32_t,uint32_t> node : c_ref) { // For each node in 'c_ref' (i.e. a node on T2 whose new parent has to be found)
                        if (child == node.first) { // If its new parent is the new node being created
                            c.pb(node.second);
                            uint32_t size_dec = 1; for (uint32_t j = 0; j < (t[t2[alpha(node.second)]]&num_c); ++j) size_dec += t[sizeOfChildOnT(t2[alpha(node.second)], j)]; // Size decrement
                            size -= size_dec; total_size -= size_dec;
                            ++total_number;
                        }
                    }
                    new_node = addNodeOnT2(t2, child, size, c);
                } else {
                    for (pair<uint32_t,uint32_t> node : c_ref) {
                        if (child == node.first) {
                            new_node = node.second;
                            ++total_number; break;
                        }
                    }
                }
                s.push(new_node); // Push new connected component to stack
            }
            // If necessary, update nodes on T2 for 'tc''s parent
            if (t2[alpha(t2c)] != tc) { // If the centroid is not the root of its cover element
                // Undo 't2c' deletion and update its parameters
                uint32_t parent = t2[parnt(t2c)];
                if (parent != t2c) ++t2[parent]; // Get back reference on 't2c''s parent
                t2[t2c] -= total_number; // Decrement number of children on T2
                t2[t2c+2] -= total_size;
                uint32_t i = 0;
                for (pair<uint32_t,uint32_t> node : c_ref) {
                    if (t2[alpha(r)] == node.first) { // If it was attached "before" than 'tc'
                        t2[childOnT2(t2c, i)] = node.second; // Then add its ID among the updated 't2c''s children
                        t2[parnt(node.second)] = t2c; // And set 't2c' as its parent
                        ++i;
                    }
                }
            }
            if (t[parnt(tc)] != tc) { s.push(r); ++nc; } // If centroid on T has a parent
            // Print node to output vectors
            ct.shape[ptr1] = 0; // Print "("
            ct.ids[ptr2] = tc; // Print centroid ID
            ++ptr1; ++ptr2;
            ct.shape[ptr1+2*(size-1)] = 1; // Print ")"
        } else { // If connected component is smaller than threshold 'B'
            struct c_tree tmp = stdCentroidDecomposition(aux_s, t, t2[alpha(r)], size);
            for (uint8_t el : tmp.shape) { ct.shape[ptr1] = el; ++ptr1; } // Copy shape
            for (uint32_t el : tmp.ids) { ct.ids[ptr2] = el; ++ptr2; } // Copy ids
        }
        while (ptr1 < ct.shape.size() && ct.shape[ptr1] == 1) ++ptr1; // Go past "closed" nodes
    }
    return ct;
}

/*
 * CORRECTNESS CHECK
 */

// Check correctness of a centroid decomposition
// @param t         T representation
// @param ct        pair<shape,ids> (struct) representation of centroid tree
// @return          true if centroid tree is correct, false otherwise
bool checkCorrectness(vector<uint32_t> &t, const struct c_tree &ct) { // Complexity: unknown and not relevant
    vector<uint32_t> roots; roots.pb(0);
    uint32_t N = sizeOfT(t);
    // struct stk noc; noc.init(N); noc.push(1);
    stack<int> noc; noc.push(1);
    uint32_t i = 0, ptr = 0;
    for (uint32_t el : ct.shape) {
        if (el == 0) { // If "("
            uint32_t id = ct.ids[ptr]; // ID of the node
            ++ptr;
            // Check correctness
            bool found = false;
            for (uint32_t j = roots.size()-noc.top(); j < roots.size(); ++j) {
                uint32_t r = roots[j];
                if (id == stdFindCentroid(t, r)) {
                    found = true;
                    roots.erase(roots.begin()+j);
                    --noc.top();
                    rmNodeOnT(t, id);
                    uint32_t c = 0;
                    for(uint32_t k = (t[id]&num_c); k > 0; --k) { roots.pb(t[childOnT(id, k-1)]); ++c; } // Centroid's children
                    if (id != r) { roots.pb(r); ++c; }
                    noc.push(c);
                    break;
                }
            }
            if (!found) return false;
        } else if (el == 1) noc.pop(); // Else, if ")"
        ++i;
    }
    return true;
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

// Convert the representation of centroid tree from pair<shape,ids> to string
// @param ct    centroid tree pair<shape,ids> (struct) representation
// @return      centroid tree string representation
inline string ctToString(const struct c_tree &ct) { // Complexity: O(n)
    oss os;
    uint32_t ptr = 0;
    for (uint32_t el : ct.shape) {
        switch (el) {
            case 0: os << "(" << ct.ids[ptr]; ++ptr; break;
            case 1: os << ")"; break;
        }
    }
    return os.str();
}

#endif