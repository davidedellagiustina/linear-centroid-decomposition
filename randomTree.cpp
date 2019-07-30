#include <iostream>
#include <sstream>
#include <time.h>
using namespace std;

int main(int argc, char* argv[]) {
    istringstream is(argv[1]);
    uint64_t n, c = 0;
    is >> n;
    ostringstream os;
    srand((unsigned int)time(0));
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