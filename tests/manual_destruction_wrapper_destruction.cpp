#include "manually_destructed.hpp"
#include <utility>

using namespace genex;

struct set_to_1_upon_destruction {
    int& to_set;

    set_to_1_upon_destruction(int& val) : to_set(val) {}

    ~set_to_1_upon_destruction() {
        to_set = 1;
    }
};

static void identity(int& val) {
    manually_destructed<set_to_1_upon_destruction> wrap(std::forward<int&>(val));

    // RAII deletes wrap which is not supposed to delete the value wrapped
}

int main() {
    int ret = 0;
    identity(ret);
    return ret;
}
