#include "node.cpp"
using namespace std;

class Tree {

    protected:

        Tree(string name) { // Complexity: O(1)
            this->name = name;
            this->size = 0;
        }

        string treeInfo() const {
            ostringstream os;
            os << "Tree name: '" << this->name << "'." << endl;
            os << "Tree size: " << this->size << " nodes." << endl;
            return os.str();
        }

        void consolidate() { // Complexity: Θ(n)
            for (auto it = this->tree.rbegin(); it != this->tree.rend(); ++it) {
                it->size = 1;
                if (it->children.size() > 0) {
                    for (auto c : it->children) {
                        it->size += this->tree[c].size;
                    }
                }
            }
        }

    public:

        string name;
        size_t size;
        vector<Node> tree;

        Tree(string name, string structure) { // Complexity: Θ(n)
            this->name = name;
            stack<int> s;
            int id = 0;
            for (int i = 0; i < structure.length(); i++) {
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
            this->size = this->tree[0].size;
        }

        ~Tree() {
            this->tree.clear();
        }

        string print() const {
            ostringstream os;
            os << this->treeInfo();
            for (auto &n : this->tree) {
                os << n.print();
            }
            return os.str();
        }

};

class T : public Tree {

    private:

        T(string name) : Tree(name) {
            this->m = 0;
        }

        void group(Node* n) { // Complexity: O(x) [where x is the number of n's children] / O(1) if n is a leaf
            if (n->size == 1) { // If n is a leaf
                n->pcsSize = 1;
            } else if (n->size > 1) { // If n is not a leaf
                int y = 0;
                int i = 0;
                vector<int> c;
                for (auto child : n->children) {
                    if (this->tree[child].covEl) continue; // If this child is already a cover element
                    y += this->tree[child].pcsSize;
                    c.push_back(child);
                    if (y >= this->m - 1) {
                        n->covEl = true;
                        n->pcsChildren.push_back(c);
                        c.clear();
                        y = 0;
                    }
                    i++;
                }
                if (n->covEl && y < this->m - 1) {
                    for (auto t : c) {
                        n->pcsChildren.back().push_back(t);
                    }
                    c.clear();
                }
                if (!n->covEl) {
                    n->pcsSize = y + 1;
                    n->pcsChildren.push_back(c);
                    c.clear();
                }
                if (n->pcsChildren.size() == 0 && y == 0) {
                    n->pcsSize = 1;
                }
            }
        }

    public:

        size_t m;

        T(string name, string structure) : Tree(name, structure) { // Complexity: Θ(n)
            this->m = floor(log2(this->size)); // m = log(n)
        }

        void cover() { // Complexity: O(n)
            for (auto it = this->tree.rbegin(); it != this->tree.rend(); ++it) {
                this->group(&(*it));
            }
            this->tree[0].covEl = true;
        }
/*
        string generateTi() const { // Complexity: O(n)
            ostringstream os;
            function<string(int)> dfs = [this,&dfs](int n)->string {
                ostringstream os;
                os << "(";
                if (this->tree[n].children.size() > 0) {
                    for (auto child : this->tree[n].children) {
                        if (this->tree[n].pcsChildren.size() > 1 && )
                        os << dfs(child);
                    }
                }
                os << ")";
                return os.str();
            };
            os << dfs(0);
            return os.str();
        }
*/
        string printCoverElements() const {
            ostringstream os;
            os << this->treeInfo();
            if (!this->tree[0].covEl) {
                os << "This tree has not been covered." << endl;
            } else {
                int i = 0;
                for (auto &n : this->tree) {
                    if (n.covEl) {
                        for (auto cover : n.pcsChildren) {
                            os << "Cover element #" << i << ": " << n.id;
                            queue<int> c;
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

};