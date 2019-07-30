#include <iostream>
#include <sstream>
#include <time.h>
using namespace std;

int main() {
    uint64_t n, c = 0;
    ostringstream os;
    srand((unsigned int)time(0));
    cin >> n;
    while (n > 0) {
        int p = rand() % 2;
        if (p == 0 && c > 1) {
            os << ")";
            c--;
        } else {
            os << "(";
            c++; n--;
        }
    }
    for (; c > 0; c--) {
        os << ")";
    }
    cout << os.str();
    return 0;
}