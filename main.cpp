#include "headers/main.hpp"
using namespace std;

string tree;

int main(int argc, char* argv[]) {

    if (argc <= 1) {
        cout << "No input file specified." << endl;
        return 0;
    }
    ifstream in(argv[1]);
    in >> tree;
    in.close();
    T t = T("T", tree);
    t.cover();
    // cout << t.printCoverElements();
    // Tree ti = Tree("Ti", t.generateTi());
    // cout << ti;

    return 0;

}