#ifndef T_CPP
#define T_CPP

#include "../headers/t.hpp"
using namespace std;

void T::cover() { // Complexity: O(n)
    function<void(uint64_t)> dfs = [this,&dfs](uint64_t n)->void {
        if (this->tree[n].children.size() > 0) {
            for (auto child : this->tree[n].children) {
                dfs(child);
            }
        }
        this->group(n);
    };
    dfs(0);
    this->tree[0].covEl = true;
}

void T::group(uint64_t n) { // Complexity: O(x) [where x is the number of n's children] / O(1) if n is a leaf
    if (this->tree[n].size == 1) { // If n is a leaf
        this->tree[n].pcsSize = 1;
    } else if (this->tree[n].size > 1) { // If n is not a leaf
        uint64_t y = 0;
        uint64_t i = 0;
        vector<uint64_t> c;
        for (auto child : this->tree[n].children) {
            if (this->tree[child].covEl) continue; // If this child is already a cover element
            y += this->tree[child].pcsSize;
            c.push_back(child);
            if (y >= this->m - 1) {
                this->tree[n].covEl = true;
                this->tree[n].pcsChildren.push_back(c);
                c.clear();
                y = 0;
            }
            i++;
        }
        if (this->tree[n].covEl && y < this->m - 1) {
            for (auto t : c) {
                this->tree[n].pcsChildren.back().push_back(t);
            }
            c.clear();
        }
        if (!this->tree[n].covEl) {
            this->tree[n].pcsSize = y + 1;
            this->tree[n].pcsChildren.push_back(c);
            c.clear();
        }
        if (this->tree[n].pcsChildren.size() == 0 && y == 0) {
            this->tree[n].pcsSize = 1;
        }
    }
}

Ti T::generateTi() { // Complexity: O(n)
    Ti ti = Ti(this->name + "i");
    stack<uint64_t> s;
    s.push(-1); // Root has parent -1
    function<uint64_t(uint64_t,bool,vector<uint64_t>)> dfs = [this,&dfs,&ti,&s](uint64_t n, bool red, vector<uint64_t> c)->uint64_t {
        uint64_t id = -1;
        if (!this->tree[n].visited || red) {
            id = ti.addNode(this->tree[n].id, s.top(), red);
            if (this->tree[n].covEl) ti.tree[id].covEl = true; // Copy root of PCSs
            s.push(id);
            if (this->tree[n].pcsChildren.size() > 1 && !red) {
                for (auto pcs : this->tree[n].pcsChildren) {
                    dfs(n, true, pcs);
                }
            }
            if (c.size() > 0) {
                vector<uint64_t> t;
                for (auto child : c) {
                    int s = dfs(child, false, this->tree[child].children);
                    if (s != -1) t.push_back(s);
                }
                if (t.size() > 0) ti.tree[id].pcsChildren.push_back(t); // Copy PCSs children
            }
            s.pop();
            this->tree[n].visited = true;
        }
        return (id == -1)? id : ((ti.tree[id].covEl)? -1 : id);
    };
    dfs(0, false, this->tree[0].children);
    ti.consolidate();
    return ti;
}

// In future should return some reference to the centroid node
void T::findCentroid() { // Complexity: O(n) [to generate structure] + ?? [should be O(n/log(n) -> check when algorithm is complete]
    this->cover();
    // cout << this->print() << endl;
    // cout << this->printCoverElements();
    Ti ti = this->generateTi();
    // cout << ti.print() << endl;
    // cout << ti.printCoverElements();
    Tii tii = ti.generateTii();
    // cout << tii.print() << endl;
    /*function<void(uint64_t)> dfs = [&dfs,&tii](uint64_t n)->void {
        cout << "Node #" << tii.tree[n].id << ":" << endl;
        cout << " - p_delta: " << tii.tree[n].p_delta << endl;
        if (tii.tree[n].children.size() > 0) {
            for (auto c_delta : tii.tree[n].c_deltas) {
                cout << " - c_delta: " << c_delta << endl;
            }
            for (auto child : tii.tree[n].children) {
                dfs(child);
            }
        }
    };*/
    // dfs(0);
    // TODO: go in the direction of the centroid
    function<bool(uint64_t)> containsCentroid = [this,&tii](uint64_t n)->bool {
        if (tii.tree[n].p_delta > this->size / 2.0) return false;
        if (tii.tree[n].c_deltas.size() > 0) {
            for (auto c_delta : tii.tree[n].c_deltas) {
                if (c_delta > this->size / 2.0) return false;
            }
        }
        return true;
    };
    function<int64_t(uint64_t)> tiiDfs = [&tii,&tiiDfs,&containsCentroid](uint64_t n)->int64_t {
        int64_t centroidSubtree;
        if (containsCentroid(n)) {
            centroidSubtree = n;
        } else if (tii.tree[n].children.size() > 0) {
            for (auto child : tii.tree[n].children) {
                int64_t h = tiiDfs(child);
                if (h != -1) centroidSubtree = h;
            }
        } else {
            centroidSubtree = -1;
        }
        return centroidSubtree;
    };
    function<bool(uint64_t)> isCentroid = [this,&ti](uint64_t n)->bool {
        if (this->size - this->tree[ti.tree[n].beta].size > this->size / 2.0) return false;
        if (this->tree[ti.tree[n].beta].children.size() > 0) {
            for (auto child : this->tree[ti.tree[n].beta].children) {
                if (this->tree[child].size > this->size / 2.0) return false;
            }
        }
        return true;
    };
    function<int64_t(uint64_t)> tiDfs = [this,&ti,&tiDfs,&isCentroid](uint64_t n)->int64_t {
        int64_t centroid;
        if (isCentroid(n)) {
            centroid = n;
        } else if (ti.tree[n].pcsChildren.size() > 0) {
            for (auto child : ti.tree[n].pcsChildren[0]) {
                int64_t h = tiDfs(child);
                if (h != -1) centroid = h;
            }
        } else {
            centroid = -1;
        }
        return centroid;
    };
    cout << ti.tree[tiDfs(tii.tree[tiiDfs(0)].alpha)].beta << endl;
}

T::T(string name, string structure) : Tree(name, structure) { // Complexity: Θ(n)
    this->m = floor(log2(this->size)); // m = log(n)
}

// In future should return the centroid tree
void T::buildCentroidTree() { // Complexity: ?? [should be O(n) -> check when algorithm is complete]
    this->findCentroid();
}

string T::printCoverElements() const {
    ostringstream os;
    os << this->treeInfo();
    if (!this->tree[0].covEl) {
        os << "This tree has not been covered." << endl;
    } else {
        uint64_t i = 0;
        for (auto &n : this->tree) {
            if (n.covEl) {
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