#include "generationally_indexed_container.hpp"
#include <utility>

using namespace genex;

int main() {
    gic<int> container;

    static const int VAL = 444;

    auto key = container.emplace(VAL);
    container.remove(key);
    int * maybe_val = container.get(key);

    if(maybe_val == nullptr)
        return 0;

    if(*maybe_val == VAL)
        return 1;

    return *maybe_val;
}
