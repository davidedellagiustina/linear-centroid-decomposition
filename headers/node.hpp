#ifndef NODE_HPP
#define NODE_HPP

using std::string;
using std::vector;

class Node {

    public:
    
        // General
        uint64_t id;
        int64_t parent;
        vector<uint64_t> children;
        size_t size = 0;
        // Used for tree covering
        int color = 0; // 0: black, 1: red
        bool covEl = false; // true when there's a cover element rooted at this node
        bool visited = false; // used during Ti generation
        size_t pcsSize = 0;
        vector<vector<uint64_t>> pcsChildren;
        int64_t alpha = -1; // associates T" nodes to T' nodes
        int64_t inv_alpha = -1; // inverse of alpha function
        int64_t beta = -1; // associates T' nodes to T nodes
        size_t weight = 0;
        int64_t p_delta;
        vector<uint64_t> c_deltas;
        // Used for centroid computation
        bool deleted = false;

        Node(uint64_t id, bool red);

        Node(uint64_t id, int64_t parent, bool red);

        ~Node();

        void addChild(uint64_t child);

        void removeChild(uint64_t child);

        string print() const;

};

#endif