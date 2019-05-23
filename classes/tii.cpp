#ifndef TII_CPP
#define TII_CPP

#include "../headers/tii.hpp"
using namespace std;

Tii::Tii(string name) : Tree() { // Complexity: O(1)
    this->name = name;
}

uint64_t Tii::addNode(uint64_t idRef, uint64_t parent) { // Complexity: O(1)
    uint64_t id = this->tree.size();
    Node* n = new Node(id, parent);
    n->alpha = idRef;
    this->tree.push_back(*n);
    if (parent != -1) this->tree[parent].addChild(id);
    return id;
}

void Tii::consolidate(vector<Node> tii) {
    function<void(uint64_t)> dfs = [this,&dfs,&tii](uint64_t n)->void {
        this->tree[n].size = tii[this->tree[n].alpha].size;
        if (this->tree[n].children.size() > 0) {
            for (auto child : this->tree[n].children) {
                dfs(child);
                this->tree[n].size -= tii[this->tree[child].alpha].size;
            }
        }
    };
    dfs(0);
    this->size = this->tree[0].size;
}

#endif