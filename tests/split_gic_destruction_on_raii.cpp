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

static void set_to_0(int & val) {
    split_gic<set_to_0_upon_destruction> container;
    auto __ = container.emplace(val);
    (void)__;
}

int main() {
    int ret = 1;
    set_to_0(ret);
    return ret;
}
