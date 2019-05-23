#ifndef TI_HPP
#define TI_HPP

#include "tii.hpp"
using std::string;

class Ti : public Tree {

    public:

        Ti(string name);

        uint64_t addNode(uint64_t idRef, uint64_t parent, bool red);

        void consolidate();

        Tii generateTii();

        string printCoverElements() const;

};

#endif