#ifndef TI_HPP
#define TI_HPP

#include "tii.hpp"
using std::string;
using std::vector;

class Ti : public Tree {

    private:

        void splitPcs(uint64_t id, Tii& tii);

    public:

        Ti();

        Ti(string name);

        uint64_t addNode(uint64_t idRef, int64_t parent, bool red = false);

        vector<uint64_t> removeNode(uint64_t id, Tii& tii);

        void consolidate();

        Tii generateTii();

        string printCoverElements() const;

};

#endif