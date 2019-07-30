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
    chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
    CentroidTree ct = t.buildCentroidTree();
    chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(t2-t1).count();
    int us = duration;
    int ms = us/1000; us %= 1000;
    int s = ms / 1000; ms %= 1000;
    int m = s/60; s %= 60;
    int h = m/60; m %= 60;
    // cout << "Execution time: " << h << "h " << m << "m " << s << "s " << ms << "ms " << us << "us";
    cout << duration;
    cout << endl;
    // cout << ct.print();
    // cout << endl;
    // cout << ct.getRepresentation();
    return 0;
}