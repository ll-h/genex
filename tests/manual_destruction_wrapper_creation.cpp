#include "manually_destructed.hpp"

using namespace genex;

int main() {
    manually_destructed<int> wrap(55);
    return wrap == 55 ? 0 : 1;
}
