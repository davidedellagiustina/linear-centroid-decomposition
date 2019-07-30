#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <stdlib.h>
using namespace std;

int main() {
    uint64_t start = 200000, end = 1000000, step = 100000;
    for (uint64_t n = start; n <= end; n += step) {
            cout << n << " nodi:" << endl;
        for (int i = 0; i < 5; i++) {
            ostringstream rt;
            system(("randomTree.exe " + to_string(n) + " > tmp.txt").c_str());
            system("main.exe tmp.txt");
        }
        cout << "--------------------" << endl;
    }
    return 0;
}