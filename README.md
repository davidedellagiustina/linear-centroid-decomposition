# A new linear-time algorithm for centroid decomposition

The centroid of a tree is a node that, when removed, breaks the tree in connected components of size at most half of that of the original tree. By recursing this procedure on the components, one obtains the centroid decomposition of the tree, also known as centroid tree. The centroid tree has logarithmic height and its construction is a powerful pre-processing step in several tree-processing algorithms. The folklore recursive algorithm for computing the centroid tree runs in <code>O(n*log(n))</code> time. To the best of our knowledge, the only result claiming <code>O(n)</code> time is unpublished and relies on (dynamic) heavy path decomposition of the original tree. In this repository, we include our implementation for the new linear-time algorithm that we proposed, based on the idea of applying the folklore algorithm to a suitable decomposition of the original tree.

# Implementation details

Both T and T'' are internally represented with a vector of 32bit integers: each node of the two trees occupies a certain portion of this vector.

In T, the _i_-th node occupies the region T[i], ..., T[j]:
 - T[i] is the number of _i_'s children, packed with a bitflag saying if it is a cover element (MSB);
 - T[i+1] is the ID of _i_'s parent;
 - T[i+2], ..., T[j] are _i_'s children. Each one occupied two integers T[h], T[h+1]:
  * T[h] is the child's ID;
  * T[h+1] is the weight of the subtree rooted at T[T[h]].

In T'', the _i_-th node occupied the region T[i], ..., T[j]:
 - T''[i] is the number of _i_'s children;
 - T''[i+1] is the ID of _i_'s parent;
 - T''[i+2] is the size of the treelet represented by the node _i_;
 - T''[i+3] is the ID of the root of the treelet on T;
 - T''[i+4], ..., T''[j] are _i_'s children. Each one occupies three integers T''[h], ..., T'''[h+2]:
  * T''[h] is the child's ID;
  * T''[h+1] and T''[h+2] are the two deltas on the edge connecting _i_ to T[h].

# Performances

_TODO: add graph of performaces_