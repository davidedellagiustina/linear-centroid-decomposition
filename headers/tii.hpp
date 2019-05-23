#ifndef TII_HPP
#define TII_HPP

using std::string;

class Tii : public Tree {

    public:

        Tii(string name);

        uint64_t addNode(uint64_t idRef, uint64_t parent);

};

#endif