#ifndef T_HPP
#define T_HPP

#include "ti.hpp"
using std::string;

class T : public Tree {

    private:

        size_t m;
        Ti ti;
        Tii tii;

        void cover();

        void group(uint64_t n);

        Ti generateTi();

        uint64_t findCentroid(uint64_t root);

    public:

        T(string name, string structure);

        // In future should return the centroid tree
        void buildCentroidTree();

        string printCoverElements() const;

};

#endif