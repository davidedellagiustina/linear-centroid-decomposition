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
    stack<int64_t> s;
    s.push(-1); // Root has parent -1
    function<int64_t(uint64_t,bool,vector<uint64_t>)> dfs = [this,&dfs,&ti,&s](uint64_t n, bool red, vector<uint64_t> c)->int64_t {
        int64_t id = -1;
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

uint64_t T::findCentroid() { // Complexity: O(n) [to generate structure] + O(n/log(n)) [to compute the centroid]
    // Generate structure
    this->cover();
    Ti ti = this->generateTi();
    Tii tii = ti.generateTii();
    // Find subtree on T" containing the centroid of T
    double s = this->size / 2.0;
    uint64_t centroidSubtree = 0, bak = centroidSubtree;
    bool found = false;
    while(!found) {
        bak = centroidSubtree;
        if (tii.tree[centroidSubtree].p_delta > s) centroidSubtree = tii.tree[centroidSubtree].parent;
        else if (tii.tree[centroidSubtree].c_deltas.size() > 0) {
            uint64_t i = 0;
            for (auto c_delta : tii.tree[centroidSubtree].c_deltas) {
                if (c_delta > s) centroidSubtree = tii.tree[centroidSubtree].children[i];
                i++;
            }
        }
        if (bak == centroidSubtree) found = true;
    }
    // Find the centroid of T
    uint64_t centroid = tii.tree[centroidSubtree].alpha;
    bak = centroid;
    found = false;
    while(!found) {
        bak = centroid;
        if (this->size - this->tree[ti.tree[centroid].beta].size > s) centroid = ti.tree[centroid].parent;
        else if (ti.tree[centroid].pcsChildren.size() > 0) {
            for (auto child : ti.tree[centroid].pcsChildren[0]) {
                if (this->tree[ti.tree[child].beta].size > s) centroid = child;
            }
        }
        if (bak == centroid) found = true;
    }
    return ti.tree[centroid].beta;
}

T::T(string name, string structure) : Tree(name, structure) { // Complexity: Θ(n)
    this->m = floor(log2(this->size)); // m = log(n)
}

// In future should return the centroid tree
void T::buildCentroidTree() { // Complexity: ?? [should be O(n) -> check when algorithm is complete]
    cout << this->findCentroid() << endl;
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