#include "split_gic.hpp"

using namespace genex;

int main() {
    split_gic<int> container;
    split_gic<int>::const_iterator it = container.cbegin();
    (void)it;
    return 0;
}
