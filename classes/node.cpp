using namespace std;

class Node {

    public:

        // General
        uint64_t id;
        uint64_t parent;
        vector<uint64_t> children;
        size_t size = 0;
        // Used for tree covering
        int color = 0; // 0: black, 1: red
        bool covEl = false; // true when there's a cover element rooted at this node
        bool visited = false; // used during Ti generation
        size_t pcsSize = 0;
        vector<vector<uint64_t>> pcsChildren;
        uint64_t alpha = -1;
        uint64_t beta = -1;

        Node(uint64_t id, bool red = false) { // Complexity: O(1)
            this->id = id;
            if (red) { this->color = 1; }
            this->parent = -1;
        }

        Node(uint64_t id, uint64_t parent, bool red = false) { // Complexity: O(1)
            this->id = id;
            if (red) { this->color = 1; }
            this->parent = parent;
        }

        ~Node() {}

        void addChild(uint64_t child) { // Complexity: O(1)
            this->children.push_back(child);
        }

        string print() const {
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
            os << endl;
            return os.str();
        }

};