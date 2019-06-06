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

void Tii::computeDeltas(vector<Node> ti) { // Complexity: O(n/log(n))
    // Compute weights
    function<void(uint64_t)> dfs = [this,&dfs,&ti](uint64_t n)->void {
        this->tree[n].weight = ti[this->tree[n].alpha].size;
        if (this->tree[n].children.size() > 0) {
            for (auto child : this->tree[n].children) {
                dfs(child);
                this->tree[n].weight -= ti[this->tree[child].alpha].size;
            }
        }
    };
    dfs(0);
    // Compute deltas (requires weights to be already computed)
    function<void(uint64_t)> deltas = [this,&deltas](uint64_t n)->void {
        // Compute p_delta
        if (this->tree[n].parent == -1) {
            this->tree[n].p_delta = -1;
        } else {
            this->tree[n].p_delta = 0;
            if (this->tree[this->tree[n].parent].parent != -1) this->tree[n].p_delta += this->tree[this->tree[n].parent].p_delta;
            this->tree[n].p_delta += this->tree[this->tree[n].parent].weight;
            for (auto child : this->tree[this->tree[n].parent].children) {
                if (this->tree[child].id != this->tree[n].id) {
                    function<int(uint64_t)> sum = [this,&sum](uint64_t n)->int {
                        int count = this->tree[n].weight;
                        if (this->tree[n].children.size() > 0) {
                            for (auto child : this->tree[n].children) {
                                count += sum(child);
                            }
                        }
                        return count;
                    };
                    this->tree[n].p_delta += sum(child);
                }
            }
        }
        // Compute c_deltas
        if (this->tree[n].children.size() > 0) {
            for (auto child : this->tree[n].children) {
                function<int(uint64_t)> sum = [this,&sum](uint64_t n)->int {
                    int count = this->tree[n].weight;
                    if (this->tree[n].children.size() > 0) {
                        for (auto child : this->tree[n].children) {
                            count += sum(child);
                        }
                    }
                    return count;
                };
                this->tree[n].c_deltas.push_back(sum(child));
            }
            // Recursion
            for (auto child : this->tree[n].children) {
                deltas(child);
            }
        }
    };
    deltas(0);
}

#endif