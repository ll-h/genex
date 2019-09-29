#include "split_gic.hpp"

#include <algorithm>

int main() {
    static int const VAL_A = 789;
    static int const VAL_B = 9999;

    genex::split_gic<int> container;
    auto key_a = container.emplace(VAL_A);
    auto key_b = container.emplace(VAL_B);

    auto ita = container.begin();
    auto itb = container.begin();
    ++itb;

    std::iter_swap(ita, itb);

    if (*ita == VAL_A) {
        return 1;
    }

    if (*itb == VAL_B) {
        return 2;
    }

    if (!(*ita == VAL_B && *itb == VAL_A))

    return 0;
}
