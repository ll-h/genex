#include "split_gic.hpp"

int main() {
    static int const FREE_VAL = 9999;
    static int const VAL = 789;

    genex::split_gic<int> container;
    auto free_key = container.emplace(FREE_VAL);
    auto sec_key = container.emplace(VAL);
    container.remove(free_key);

    auto it = container.begin();

    if (*it == FREE_VAL) {
        return 1;
    }

    if (*it != VAL) {
        return 2;
    }

    return 0;
}
