#ifndef T_HPP
#define T_HPP

#include "ti.hpp"
using std::string;

class T : public Tree {

    private:

        size_t m;

        void cover();

        void group(uint64_t n);

        Ti generateTi();

        // In future should return some reference to the centroid node
        void findCentroid();

    public:

        T(string name, string structure);

        // In future should return the centroid tree
        void buildCentroidTree();

        string printCoverElements() const;

};

#endif