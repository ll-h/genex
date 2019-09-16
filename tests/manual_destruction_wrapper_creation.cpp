#include "generationally_indexed_container.hpp"

using namespace genex;

int main() {
    manual_destruction_wrapper<int> wrap(55);
    return wrap == 55 ? 0 : 1;
}
