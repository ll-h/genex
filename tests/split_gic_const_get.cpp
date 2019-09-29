#include "generationally_indexed_container.hpp"
#include <utility>

using namespace genex;

static const int VAL = 444;

static int check_from_const(split_gic<int> const &container, split_gic<int>::key_type const &k) {
    // testing call to get on a const gic
    int const * maybe_val = container.get(k);

    if(maybe_val == nullptr)
        return 1;

    if(*maybe_val != VAL)
        return 2;

    // testing call to the operator [] on a const gic
    maybe_val = container[k];

    if(maybe_val == nullptr)
        return 3;

    if(*maybe_val != VAL)
        return 4;

    return 0;
}

int main() {
    split_gic<int> container;

    auto key = container.emplace(VAL);
    return check_from_const(container, key);
}
