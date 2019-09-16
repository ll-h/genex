#include "generationally_indexed_container.hpp"

int main() {
    genex::manual_destruction_wrapper<int> ret(1);
    ret.erase();
    ret.emplace(0);
    return ret;
}
