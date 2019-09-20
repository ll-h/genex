#include "generationally_indexed_container.hpp"

using namespace genex;

int main() {
    gic<int> container;
    gic<int>::const_iterator it = container.cbegin();
    (void)it;
    return 0;
}
