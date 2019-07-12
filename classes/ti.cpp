#ifndef TI_CPP
#define TI_CPP

#include "../headers/ti.hpp"
using namespace std;

void Ti::splitPcs(uint64_t id) {
    // Find root of PCS
    uint64_t root = id;
    while (!this->tree[root].covEl) {
        root = this->tree[root].parent;
    }
    // Split PCS into singleton trees
    function<void(uint64_t)> dfs = [this,&dfs](uint64_t id)->void {
        if (this->tree[id].pcsChildren.size() > 0) {
            for (auto child : this->tree[id].pcsChildren[0]) {
                dfs(child);
            }
        }
        this->tree[id].pcsChildren.clear();
        this->tree[id].covEl = true;
    };
    dfs(root);
}

Ti::Ti() : Tree() {}

Ti::Ti(string name) : Tree() { // Complexity: O(1)
    this->name = name;
    this->roots.push_back(0);
}

uint64_t Ti::addNode(uint64_t idRef, int64_t parent, bool red = false) { // Complexity: O(1)
    uint64_t id = this->tree.size();
    Node* n = new Node(id, parent, red);
    n->beta = idRef;
    this->tree.push_back(*n);
    if (parent != -1) this->tree[parent].addChild(id);
    return id;
}

void Ti::removeNode(uint64_t id, bool look_up) { // Complexity: O(x) where x = c + pc (c = number of children, pc = number of children of parent)
    uint64_t beta = this->tree[id].beta;
    if (this->tree[id].color == 0) { // If node is black
        if (this->tree[id].children.size() > 0) {
            for (auto child : this->tree[id].children) {
                if (this->tree[child].color == 1) {
                    if (this->tree[child].children.size() > 0) {
                        for (auto c : this->tree[child].children) {
                            this->tree[c].parent = -1;
                        }
                    }
                    this->splitPcs(child);
                    this->tree[child].deleted = true;
                }
            }
        }
        this->splitPcs(id);
        this->tree[id].deleted = true;
    } else { // If node is red
        if (this->tree[id].children.size() > 0) {
            for (auto child : this->tree[id].children) {
                this->tree[child].parent = -1;
            }
        }
        this->splitPcs(id);
        this->tree[id].deleted = true;
        if (this->tree[this->tree[id].parent].children.size() > 0) {
            for (auto child : this->tree[this->tree[id].parent].children) {
                if (child != id && this->tree[child].color == 1) {
                    if (this->tree[child].children.size() > 0) {
                        for (auto c : this->tree[child].children) {
                            this->tree[c].parent = -1;
                        }
                    }
                    this->splitPcs(child);
                    this->tree[child].deleted = true;
                }
            }
        }
        this->splitPcs(this->tree[id].parent);
        this->tree[this->tree[id].parent].deleted = true;
    }
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
    stack<int64_t> s;
    s.push(-1); // Root has parent -1
    function<void(uint64_t)> dfs = [this,&dfs,&tii,&s](uint64_t n)->void {
        int64_t id = -1;
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
            if (n.covEl && !n.deleted) {
                if (n.pcsChildren.size() == 0) {
                    os << "Cover element #" << i << ": " << n.id << endl;
                    i++;
                }
                for (auto cover : n.pcsChildren) {
                    os << "Cover element #" << i << ":";
                    function<void(uint64_t,vector<uint64_t>)> dfs = [this,&dfs,&os](uint64_t n, vector<uint64_t> c)->void  {
                        os << " " << this->tree[n].id;
                        if (c.size() > 0) {
                            for (auto child : c) {
                                vector<uint64_t> h;
                                if (this->tree[child].pcsChildren.size() > 0) h = this->tree[child].pcsChildren[0];
                                dfs(child, h);
                            }
                        }
                    };
                    dfs(n.id, cover);
                    os << endl;
                    i++;
                }
            }
        }
    }
    return os.str();
}

#endif