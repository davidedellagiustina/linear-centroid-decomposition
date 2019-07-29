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

uint64_t T::findCentroid(uint64_t root) { // Complexity: O(n/log(n))
    double s = this->tree[this->ti.tree[root].beta].size / 2.0;
    root = this->ti.tree[root].inv_alpha;
    uint64_t centroidSubtree = root, bak;
    bool found = false;
    while(!found) {
        bak = centroidSubtree;
        if (this->tii.tree[centroidSubtree].p_delta > s) centroidSubtree = this->tii.tree[centroidSubtree].parent;
        else if (this->tii.tree[centroidSubtree].c_deltas.size() > 0) {
            uint64_t i = 0;
            for (auto c_delta : this->tii.tree[centroidSubtree].c_deltas) {
                if (c_delta > s) centroidSubtree = this->tii.tree[centroidSubtree].children[i];
                i++;
            }
        }
        if (bak == centroidSubtree) found = true;
    }
    // Find the centroid of T
    uint64_t centroid = this->tii.tree[centroidSubtree].alpha;
    size_t size = this->tree[this->ti.tree[centroid].beta].size;
    found = false;
    while(!found) {
        bak = centroid;
        if (size - this->tree[this->ti.tree[centroid].beta].size > s) centroid = this->ti.tree[centroid].parent;
        else if (this->ti.tree[centroid].pcsChildren.size() > 0) {
            for (auto child : this->ti.tree[centroid].pcsChildren[0]) {
                if (this->tree[this->ti.tree[child].beta].size > s) centroid = child;
            }
        }
        if (bak == centroid) found = true;
    }
    return centroid; // One of the images of T centroid in T'
}

T::T(string name, string structure) : Tree(name, structure) { // Complexity: Θ(n)
    this->m = floor(log2(this->size)); // m = log(n)
    this->cover();
    this->ti = this->generateTi();
    this->tii = this->ti.generateTii();
}

void T::removeNode(uint64_t id) { // Complexity: O(1)
    if (this->tree[id].parent != -1) {
        this->tree[this->tree[id].parent].removeChild(id);
        // Update parent size (and up)
        int64_t parent = this->tree[id].parent;
        while(parent != -1) {
            this->tree[parent].size -= this->tree[id].size;
            parent = this->tree[parent].parent;
        }
    }
    this->tree[id].deleted = true;
    this->size--;
    if (this->tree[id].children.size() > 0) {
        for (auto child : this->tree[id].children) {
            this->tree[child].parent = -1;
        }
    }
}

CentroidTree T::buildCentroidTree() { // Complexity: ?? [should be O(n) -> check when algorithm is complete]
    CentroidTree ct = CentroidTree("Centroid Tree");
    stack<int64_t> s = stack<int64_t>();
    s.push(-1);
    function<void(uint64_t)> find = [this,&find,&ct,&s](uint64_t root)->void {
        uint64_t centroid = this->findCentroid(root);
        cout << "centroid: " << centroid << endl;
        uint64_t id = ct.addNode(this->ti.tree[centroid].beta, s.top());
        vector<uint64_t> roots = this->ti.removeNode(centroid, this->tii);
        this->removeNode(this->ti.tree[centroid].beta);
        cout << "Removed node " << this->ti.tree[centroid].beta << endl;
        cout << this->print() << endl;
        cout << this->ti.print() << endl;
        // cout << "Tii roots:";
        // for (auto root : this->tii.roots) {
        //     cout << " " << root;
        // }
        // cout << endl;
        cout << this->tii.print() << endl;
        cout << "roots: ";
        for (uint64_t root : roots) {
            cout << root << " ";
        }
        cout << endl;
        s.push(id);
        int i = 0;
        for (uint64_t root : roots) {
            cout << "root " << i << ": " << root << endl;
            find(root);
            i++;
        }
        s.pop();
    };
    find(0);
    return ct;
}

string T::printCoverElements() const {
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