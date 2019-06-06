#ifndef TI_CPP
#define TI_CPP

#include "../headers/ti.hpp"
using namespace std;

Ti::Ti(string name) : Tree() { // Complexity: O(1)
    this->name = name;
}

uint64_t Ti::addNode(uint64_t idRef, uint64_t parent, bool red = false) { // Complexity: O(1)
    uint64_t id = this->tree.size();
    Node* n = new Node(id, parent, red);
    n->beta = idRef;
    this->tree.push_back(*n);
    if (parent != -1) this->tree[parent].addChild(id);
    return id;
}

void Ti::consolidate() { // Complexity: Θ(n)
    function<void(uint64_t)> dfs = [this,&dfs](uint64_t n)->void {
        this->tree[n].size = ((this->tree[n].color == 0)? 1 : 0);
        if (this->tree[n].children.size() > 0) {
            for (auto child : this->tree[n].children) {
                dfs(child);
                this->tree[n].size += this->tree[child].size;
            }
        }
    };
    dfs(0);
    this->size = this->tree[0].size;
}

Tii Ti::generateTii() { // Complexity: O(n)
    Tii tii = Tii(this->name + "i");
    stack<uint64_t> s;
    s.push(-1); // Root has parent -1
    function<void(uint64_t)> dfs = [this,&dfs,&tii,&s](uint64_t n)->void {
        uint64_t id = -1;
        if (this->tree[n].covEl) {
            id = tii.addNode(n, s.top());
        }
        if (id != -1) s.push(id);
        if (this->tree[n].children.size() > 0) {
            for (auto child : this->tree[n].children) {
                dfs(child);
            }
        }
        if (id != -1) s.pop();
    };
    dfs(0);
    tii.consolidate();
    tii.computeDeltas(this->tree);
    return tii;
}

string Ti::printCoverElements() const {
    ostringstream os;
    os << this->treeInfo();
    if (!this->tree[0].covEl) {
        os << "This tree has not been covered." << endl;
    } else {
        uint64_t i = 0;
        for (auto &n : this->tree) {
            if (n.covEl) {
                if (n.pcsChildren.size() == 0) {
                    os << "Cover element #" << i << ": " << n.id << endl;
                    i++;
                }
                for (auto cover : n.pcsChildren) {
                    os << "Cover element #" << i << ": " << n.id;
                    queue<uint64_t> c;
                    for (auto n : cover) {
                        c.push(n);
                    }
                    while (!c.empty()) {
                        os << " " << c.front();
                        if (this->tree[c.front()].pcsChildren.size() > 0) {
                            for (auto n : this->tree[c.front()].pcsChildren[0]) {
                                c.push(this->tree[n].id);
                            }
                        }
                        c.pop();
                    }
                    os << endl;
                    i++;
                }
            }
        }
    }
    return os.str();
}

#endif