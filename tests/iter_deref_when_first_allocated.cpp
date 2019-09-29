#include "generationally_indexed_container.hpp"

int main() {
    static int const VAL = 789;
    genex::split_gic<int> container;
    auto __ = container.emplace(VAL);

    auto it = container.begin();
    if (*it != VAL) {
        return 1;
    }

    return 0;
}
