#include "generationally_indexed_container.hpp"

using namespace genex;

int main() {
    split_gic<int> container;

    static const int OLD_VAL = 444;
    static const int NEW_VAL = 999;

    auto old_key = container.emplace(OLD_VAL);
    container.remove(old_key);
    auto new_key = container.emplace(NEW_VAL);
    (void)new_key;
    int * maybe_val = container.get(old_key);

    if(maybe_val == nullptr)
        return 0;

    return *maybe_val;
}
