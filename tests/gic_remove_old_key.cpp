#include "generationally_indexed_container.hpp"
#include <utility>

using namespace genex;

int main() {
    split_gic<int> container;

    static const int OLD_VAL = 444;
    static const int NEW_VAL = 999;

    auto old_key = container.emplace(OLD_VAL);
    container.remove(old_key);
    auto new_key = container.emplace(NEW_VAL);
    container.remove(old_key); // should have no effect
    int * maybe_val = container.get(new_key);

    if(maybe_val == nullptr)
        return 1;

    if(*maybe_val == OLD_VAL)
        return 2;

    if(*maybe_val == NEW_VAL)
        return 0;

    return *maybe_val;
}
