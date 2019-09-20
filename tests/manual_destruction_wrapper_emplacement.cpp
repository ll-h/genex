#include "manually_destructed.hpp"

int main() {
    genex::manually_destructed<int> ret(1);
    ret.erase();
    ret.emplace(0);
    return ret;
}
