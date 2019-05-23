#ifndef NODE_CPP
#define NODE_CPP

#include "../headers/node.hpp"
using namespace std;

Node::Node(uint64_t id, bool red = false) { // Complexity: O(1)
    this->id = id;
    if (red) { this->color = 1; }
    this->parent = -1;
}

Node::Node(uint64_t id, uint64_t parent, bool red = false) { // Complexity: O(1)
    this->id = id;
    if (red) { this->color = 1; }
    this->parent = parent;
}

Node::~Node() {}

void Node::addChild(uint64_t child) { // Complexity: O(1)
    this->children.push_back(child);
}

string Node::print() const {
    ostringstream os;
    os << "Node #" << this->id;
    if (this->color == 1) {
        os << " (red)";
    }
    if (this->alpha != -1) {
        os << " (alpha: " << this->alpha << ")";
    }
    if (this->beta != -1) {
        os << " (beta: " << this->beta << ")";
    }
    if (this->parent != -1) {
        os << " has parent #" << this->parent << ".";
    } else {
        os << " is the root of the tree.";
    }
    // Temp
    os << " Size: " << this->size;
    //
    os << endl;
    return os.str();
}

#endif