#ifndef CT_HPP
#define CT_HPP

using std::string;

class CentroidTree : public Tree {

    public:

        CentroidTree(string name);

        uint64_t addNode(uint64_t idRef, int64_t parent);

        string print() const;

        string getRepresentation() const;

};

#endif