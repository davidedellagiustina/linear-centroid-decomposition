#ifndef TREE_CPP
#define TREE_CPP

#include "../headers/tree.hpp"
using namespace std;

string Tree::treeInfo() const {
    ostringstream os;
    os << "Tree name: '" << this->name << "'." << endl;
    os << "Tree size: " << this->size << " nodes." << endl;
    return os.str();
}

Tree::Tree() { // Complexity: O(1)
    this->name = "";
    this->size = 0;
}

Tree::Tree(string name, string structure) { // Complexity: Θ(n)
    this->name = name;
    stack<uint64_t> s;
    uint64_t id = 0;
    for (uint64_t i = 0; i < structure.length(); i++) {
        if (structure[i] == '(') {
            Node* n;
            if (s.empty()) {
                n = new Node(id);
            } else {
                n = new Node(id, s.top());
                this->tree[s.top()].addChild(id);
            }
            this->tree.push_back(*n);
            s.push(this->tree.size() - 1);
            id++;
        } else if (structure[i] == ')') {
            s.pop();
        }
    }
    this->consolidate();
}

Tree::~Tree() {
    this->tree.clear();
}

void Tree::consolidate() { // Complexity: Θ(n)
    function<void(uint64_t)> dfs = [this,&dfs](uint64_t n)->void {
        this->tree[n].size = 1;
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

string Tree::print() const { // Comprexity: O(n)
    ostringstream os;
    os << this->treeInfo();
    for (auto &n : this->tree) {
        os << n.print();
    }
    return os.str();
}

#endif