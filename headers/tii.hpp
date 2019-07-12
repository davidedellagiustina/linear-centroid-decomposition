#ifndef TII_HPP
#define TII_HPP

using std::string;
using std::vector;

class Tii : public Tree {

    public:

        vector<uint64_t> roots;

        Tii();

        Tii(string name);

        uint64_t addNode(uint64_t idRef, int64_t parent);

        void computeDeltas(vector<Node> ti);

};

#endif