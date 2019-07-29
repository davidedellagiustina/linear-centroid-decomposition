#ifndef CT_CPP
#define CT_CPP

#include "../headers/ct.hpp"
using namespace std;

CentroidTree::CentroidTree(string name) { // Complexity: O(1)
    this->name = name;
    this->size = 0;
}

uint64_t CentroidTree::addNode(uint64_t idRef, int64_t parent) { // Complexity: O(1)
    uint64_t id = this->tree.size();
    Node* n = new Node(id, parent);
    n->idRef = idRef;
    this->tree.push_back(*n);
    this->size++;
    if (parent != -1) this->tree[parent].addChild(id);
    return id;
}

string CentroidTree::print() const { // Comprexity: O(n)
    ostringstream os;
    os << this->treeInfo();
    for (auto &n : this->tree) {
        if (!n.deleted) {
            os << "Node #" << n.idRef;
            if (n.parent != -1) {
                os << " has parent #" << this->tree[n.parent].idRef << ".";
            } else {
                os << " is the root of the tree.";
            }
            os << endl;
        }
    }
    return os.str();
}

string CentroidTree::getRepresentation() const { // Complexity: O(n)
    ostringstream os;
    function<void(uint64_t)> dfs = [this,&dfs,&os](uint64_t id)->void {
        os << "(" << this->tree[id].idRef;
        if (this->tree[id].children.size() > 0) {
            for (auto child : this->tree[id].children) {
                dfs(child);
            }
        }
        os << ")";
    };
    dfs(0);
    return os.str();
}

#endif