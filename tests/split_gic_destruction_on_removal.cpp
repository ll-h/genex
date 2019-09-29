#include "split_gic.hpp"
#include <utility>

using namespace genex;

struct set_to_0_upon_destruction {
    int& to_set;

    set_to_0_upon_destruction(int& val) : to_set(val) {}

    ~set_to_0_upon_destruction() {
        to_set = 0;
    }
};

int main() {
    int ret = 1;

    split_gic<set_to_0_upon_destruction> container;
    auto key = container.emplace(ret);
    container.remove(key);

    // perform the equality check before the end of the scope (before RAII)
    if(ret == 0)
        return 0;

    return ret;
}
