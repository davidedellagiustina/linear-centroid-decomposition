using namespace std;

class Node {

    public:

        int id;
        int color = 0; // 0: black, 1: red
        size_t size = 0;
        bool covEl = false; // true when there's a cover element rooted at this node
        size_t pcsSize = 0;
        vector<vector<int>> pcsChildren;
        int parent;
        vector<int> children;

        Node(int id, bool red = false) { // Complexity: O(1)
            this->id = id;
            if (red) { this->color = 1; }
            this->parent = -1;
        }

        Node(int id, int parent, bool red = false) { // Complexity: O(1)
            this->id = id;
            if (red) { this->color = 1; }
            this->parent = parent;
        }

        ~Node() {
        }

        void addChild(int child) {
            this->children.push_back(child);
        }

        string print() const {
            ostringstream os;
            os << "Node #" << this->id;
            if (this->parent != -1) {
                os << " has parent #" << this->parent << ".";
            } else {
                os << " is the root of the tree.";
            }
            os << endl;
            return os.str();
        }

};