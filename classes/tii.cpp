#ifndef TII_CPP
#define TII_CPP

#include "../headers/tii.hpp"
using namespace std;

Tii::Tii() : Tree() {}

Tii::Tii(string name) : Tree() { // Complexity: O(1)
    this->name = name;
    this->roots.push_back(0);
}

uint64_t Tii::addNode(uint64_t idRef, int64_t parent) { // Complexity: O(1)
    uint64_t id = this->tree.size();
    Node* n = new Node(id, parent);
    n->alpha = idRef;
    this->tree.push_back(*n);
    if (parent != -1) this->tree[parent].addChild(id);
    return id;
}

void Tii::removeNode(uint64_t id) { // Complexity: O(x) where x is the number of children
    // Remove children references
    if (this->tree[id].children.size() > 0) {
        for (auto child : this->tree[id].children) {
            this->tree[child].parent = -1;
        }
    }
    // Delete node
    this->tree[id].deleted = true;
    // Remove parent reference
    if (this->tree[id].parent != -1) {
        this->tree[this->tree[id].parent].removeChild(id);
    }
}

void Tii::removeRoot(uint64_t id) { // Complexity: O(roots)
    for (auto it = this->roots.begin(); it != this->roots.end(); it++) {
        if (*it == id) this->roots.erase(it);
    }
}

void Tii::computeDeltas(vector<Node> ti) { // Complexity: O(n/log(n))
    // Compute weights and c_deltas
    function<void(uint64_t)> weightsAndCDeltas = [this,&weightsAndCDeltas,&ti](uint64_t n)->void { // Complexity: O(n/log(n))
        this->tree[n].weight = ti[this->tree[n].alpha].size;
        if (this->tree[n].children.size() > 0) {
            for (auto child : this->tree[n].children) {
                weightsAndCDeltas(child);
                this->tree[n].weight -= ti[this->tree[child].alpha].size;
                uint64_t c = this->tree[child].weight;
                if (this->tree[child].c_deltas.size() > 0) {
                    for (auto c_delta : this->tree[child].c_deltas) {
                        c += c_delta;
                    }
                }
                this->tree[n].c_deltas.push_back(c);
            }
        }

    };
    weightsAndCDeltas(0);
    // Compute p_deltas (requires weights and c_deltas to be already computed)
    function<void(uint64_t)> pDeltas = [this,&pDeltas](uint64_t n)->void { // Complexity: O(n/log(n))
        if (this->tree[n].parent == -1) {
            this->tree[n].p_delta = -1;
        } else {
            this->tree[n].p_delta = 0;
            this->tree[n].p_delta += this->tree[this->tree[n].parent].weight;
            if (this->tree[this->tree[n].parent].parent != -1) this->tree[n].p_delta += this->tree[this->tree[n].parent].p_delta;
            uint64_t i = 0;
            for (auto child : this->tree[this->tree[n].parent].children) {
                if (this->tree[child].id != this->tree[n].id) {
                    this->tree[n].p_delta += this->tree[this->tree[n].parent].c_deltas[i];
                }
                i++;
            }
        }
        // Recursion
        if (this->tree[n].children.size() > 0) {
            for (auto child : this->tree[n].children) {
                pDeltas(child);
            }
        }
    };
    pDeltas(0);
}

#endif