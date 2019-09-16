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

int main() {
    int ret = 1;
    manual_destruction_wrapper<set_to_0_upon_destruction> wrap(std::forward<int&>(ret));
    wrap.erase();
    return ret;
}
