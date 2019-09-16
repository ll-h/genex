#include "generationally_indexed_container.hpp"
#include <utility>

using namespace genex;

struct set_to_1_upon_destruction {
    int& to_set;

    set_to_1_upon_destruction(int& val) : to_set(val) {}

    ~set_to_1_upon_destruction() {
        to_set = 1;
    }
};

void identity(int& val) {
    manual_destruction_wrapper<set_to_1_upon_destruction> wrap(std::forward<int&>(val));

    // RAII deletes wrap which is not supposed to delete the value wrapped
}

int main() {
    int ret = 0;
    identity(ret);
    return ret;
}
