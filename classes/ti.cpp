#ifndef TI_CPP
#define TI_CPP

#include "../headers/ti.hpp"
using namespace std;

void Ti::splitPcs(uint64_t id, Tii& tii) { // Complexity: O(x) where x is the number of nodes of the PCS
    // Find root of PCS
    uint64_t root = id;
    while (!this->tree[root].covEl) {
        root = this->tree[root].parent;
    }
    // Delete node on T"
    tii.removeNode(this->tree[root].inv_alpha);
    tii.removeRoot(this->tree[root].inv_alpha);
    // Split PCS into singleton trees
    stack<uint64_t> s;
    s.push(-1); // Root has parent -1
    function<void(uint64_t)> dfs = [this,&dfs,&s,&tii](uint64_t id)->void {
        vector<vector<uint64_t>> pc = this->tree[id].pcsChildren; // Backup to visit children
        vector<uint64_t> c = this->tree[id].children;
        this->tree[id].pcsChildren.clear();
        this->tree[id].covEl = true;
        if (!this->tree[id].deleted) {
            // Create new node on T"
            uint64_t n = tii.addNode(id, s.top());
            if (s.top() == -1) tii.roots.push_back(n);
            this->tree[id].inv_alpha = n;
            s.push(n);
        }
        if (pc.size() > 0) {
            for (auto child : pc[0]) {
                dfs(child);
            }
            if (c.size() > pc[0].size()) {
                for (auto child : c) {
                    if (find(pc[0].begin(), pc[0].end(), child) == pc[0].end()) {
                        tii.tree[this->tree[child].inv_alpha].parent = this->tree[id].inv_alpha; // Fix disconnected but already existing nodes in T"
                        tii.tree[this->tree[id].inv_alpha].addChild(this->tree[child].inv_alpha);
                        tii.removeRoot(this->tree[child].inv_alpha);
                    }
                }
            }
        }
        s.pop();
    };
    dfs(root);
}

Ti::Ti() : Tree() {}

Ti::Ti(string name) : Tree() { // Complexity: O(1)
    this->name = name;
}

uint64_t Ti::addNode(uint64_t idRef, int64_t parent, bool red) { // Complexity: O(1)
    uint64_t id = this->tree.size();
    Node* n = new Node(id, parent, red);
    n->beta = idRef;
    this->tree.push_back(*n);
    this->size++;
    if (parent != -1) this->tree[parent].addChild(id);
    return id;
}

vector<uint64_t> Ti::removeNode(uint64_t id, Tii& tii) { // Complexity: O(x) where x is the number of children of parent
    vector<uint64_t> roots;
    if (this->tree[id].color == 0) { // If node is black
        if (this->tree[id].children.size() > 0) {
            for (auto child : this->tree[id].children) {
                if (this->tree[child].color == 1) {
                    if (this->tree[child].children.size() > 0) {
                        for (auto c : this->tree[child].children) {
                            this->tree[c].parent = -1;
                            if (!this->tree[c].deleted) roots.push_back(c);
                        }
                    }
                    this->tree[child].deleted = true;
                    this->size--;
                    this->splitPcs(child, tii);
                } else {
                    this->tree[child].parent = -1;
                    if (!this->tree[child].deleted) roots.push_back(child);
                }
            }
        }
        this->tree[id].deleted = true;
        this->size--;
        if (this->tree[id].parent != -1) {
            this->tree[this->tree[id].parent].removeChild(id);
            // Update parent size (and up)
            int64_t parent = this->tree[id].parent;
            while(parent != -1) {
                this->tree[parent].size -= this->tree[id].size;
                parent = this->tree[parent].parent;
            }
        }
        this->splitPcs(id, tii);
        tii.computeDeltas(this->tree);
        // Eventually add "general" root
        int64_t parent = this->tree[id].parent;
        uint64_t old = id;
        while(parent != -1) {
            old = parent;
            parent = this->tree[parent].parent;
        }
        if (old != id && !this->tree[old].deleted) roots.push_back(old);
    } else { // If node is red
        roots = this->removeNode(this->tree[id].parent, tii);
    }
    return roots;
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
            this->tree[n].inv_alpha = id;
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