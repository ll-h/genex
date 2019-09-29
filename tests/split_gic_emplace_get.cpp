#include "split_gic.hpp"
#include <utility>

using namespace genex;

int main() {
    split_gic<int> container;

    static const int VAL = 444;

    auto key = container.emplace(VAL);
    int * maybe_val = container.get(key);

    if(maybe_val == nullptr)
        return 1;

    if(*maybe_val != VAL)
        return 2;

    return 0;
}
