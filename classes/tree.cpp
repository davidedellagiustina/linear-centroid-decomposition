#include "node.cpp"
using namespace std;

class Tree {

    protected:

        string treeInfo() const {
            ostringstream os;
            os << "Tree name: '" << this->name << "'." << endl;
            os << "Tree size: " << this->size << " nodes." << endl;
            return os.str();
        }

    public:

        string name;
        size_t size;
        vector<Node> tree;

        Tree() { // Complexity: O(1)
            this->name = "";
            this->size = 0;
        }

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
        }

        ~Tree() {
            this->tree.clear();
        }

        void consolidate() { // Complexity: Θ(n)
            function<void(int)> dfs = [this,&dfs](int n)->void {
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

        string print() const {
            ostringstream os;
            os << this->treeInfo();
            for (auto &n : this->tree) {
                os << n.print();
            }
            return os.str();
        }

};

class Tii : public Tree {

    public:

        unordered_map<int,int> alpha;

        Tii(string name) : Tree() { // Complexity: O(1)
            this->name = name;
        }

};

class Ti : public Tree {

    public:

        unordered_map<int,int> beta;

        Ti(string name) : Tree() { // Complexity: O(1)
            this->name = name;
        }

        int addNode(int idRef, int parent, bool red = false) { // Complexity: O(1)
            int id = this->tree.size();
            Node* n = new Node(id, parent, red);
            this->tree.push_back(*n);
            this->tree[parent].addChild(id);
            this->beta.insert({id, idRef});
            return id;
        }

        string printCoverElements() const {
            ostringstream os;
            os << this->treeInfo();
            if (!this->tree[0].covEl) {
                os << "This tree has not been covered." << endl;
            } else {
                int i = 0;
                for (auto &n : this->tree) {
                    if (n.covEl) {
                        if (n.pcsChildren.size() == 0) {
                            os << "Cover element #" << i << ": " << n.id << endl;
                            i++;
                        }
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

class T : public Tree {

    private:

        size_t m;

        void cover() { // Complexity: O(n)
            function<void(int)> dfs = [this,&dfs](int n)->void {
                if (this->tree[n].children.size() > 0) {
                    for (auto child : this->tree[n].children) {
                        dfs(child);
                    }
                }
                this->group(&(this->tree[n]));
            };
            dfs(0);
            this->tree[0].covEl = true;
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

        Ti generateTi() {
            Ti ti = Ti(this->name + "i");
            stack<int> s;
            s.push(-1); // Root has parent -1
            function<int(int,bool,vector<int>)> dfs = [this,&dfs,&ti,&s](int n, bool red, vector<int> c)->int {
                int id = -1;
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
                        vector<int> t;
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
        void findCentroid() { // Complexity: ?? [should be O(n/log(n)]
            this->cover();
            // cout << this->print();
            // cout << this->printCoverElements();
            Ti ti = this->generateTi();
            // cout << ti.print();
            // for (int i = 0; i < ti.beta.size(); i++) {
            //     cout << i << " " << ti.beta[i] << " " << endl;
            // }
            cout << ti.printCoverElements();
        }

    public:

        T(string name, string structure) : Tree(name, structure) { // Complexity: Θ(n)
            this->m = floor(log2(this->size)); // m = log(n)
        }

        // In future should return the centroid tree
        void buildCentroidTree() { // Complexity: ?? [should be O(n)]
            this->findCentroid();
        }

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