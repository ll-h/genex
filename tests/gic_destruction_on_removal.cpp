#include "generationally_indexed_container.hpp"
#include <utility>

using namespace genex;

struct set_to_0_upon_destruction {
    int& to_set;

    set_to_0_upon_destruction(int& val) : to_set(val) {}

    ~set_to_0_upon_destruction() {
        to_set = 0;
    }
};

void set_to_0(int & val) {
}

int main() {
    int ret = 1;

    gic<set_to_0_upon_destruction> container;
    auto key = container.emplace(ret);
    container.remove(key);

    // perform the equality check before the end of the scope (before RAII)
    if(ret == 0)
        return 0;

    return ret;
}
