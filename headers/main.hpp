#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <functional>
#include <cmath>
#include "../classes/tree.cpp"
using namespace std;

inline ostream& operator << (ostream& os, const Tree& t) {
    return os << t.print();
}