#ifndef MAIN_CPP
#define MAIN_CPP
#endif

#include "headers/main.hpp"
using namespace std;

string tree;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "No input file specified." << endl;
        return 0;
    }
    ifstream in(argv[1]);
    in >> tree;
    in.close();
    T t = T("T", tree);
    CentroidTree ct = t.buildCentroidTree();
    cout << ct.print();
    // cout << ct.getRepresentation();
    return 0;

}