#ifndef TREE_HPP
#define TREE_HPP

using std::string;
using std::vector;

class Tree {

    protected:

        string treeInfo() const;

    public:

        string name;
        size_t size;
        vector<Node> tree;

        Tree();

        Tree(string name, string structure);

        ~Tree();

        void consolidate();

        string print() const;

};

#endif