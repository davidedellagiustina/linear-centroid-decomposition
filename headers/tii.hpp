#ifndef TII_HPP
#define TII_HPP

using std::string;
using std::vector;

class Tii : public Tree {

    public:

        Tii(string name);

        uint64_t addNode(uint64_t idRef, int64_t parent);

        void computeDeltas(vector<Node> ti);

};

#endif