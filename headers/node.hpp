#ifndef NODE_HPP
#define NODE_HPP

using std::string;
using std::vector;

class Node {

    public:
    
        // General
        uint64_t id;
        uint64_t parent;
        vector<uint64_t> children;
        size_t size = 0;
        // Used for tree covering
        int color = 0; // 0: black, 1: red
        bool covEl = false; // true when there's a cover element rooted at this node
        bool visited = false; // used during Ti generation
        size_t pcsSize = 0;
        vector<vector<uint64_t>> pcsChildren;
        uint64_t alpha = -1;
        uint64_t beta = -1;

        Node(uint64_t id, bool red);

        Node(uint64_t id, uint64_t parent, bool red);

        ~Node();

        void addChild(uint64_t child);

        string print() const;

};

#endif