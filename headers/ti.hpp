#ifndef TI_HPP
#define TI_HPP

#include "tii.hpp"
using std::string;

class Ti : public Tree {

    private:

        void splitPcs(uint64_t id, Tii& tii);

        void removeRoot(uint64_t id);

    public:

        vector<uint64_t> roots;

        Ti();

        Ti(string name);

        uint64_t addNode(uint64_t idRef, int64_t parent, bool red = false);

        void removeNode(uint64_t id, Tii& tii);

        void consolidate();

        Tii generateTii();

        string printCoverElements() const;

};

#endif