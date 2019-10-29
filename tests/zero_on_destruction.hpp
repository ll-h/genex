#ifndef ZERO_ON_DESTRUCTION_HPP
#define ZERO_ON_DESTRUCTION_HPP

#include <utility>
#include <functional>

template<typename T>
class zero_on_destruction {
private:
    T& to_zero;

public:
    zero_on_destruction(T& ref) : to_zero(ref) {}

    ~zero_on_destruction() {
        to_zero = T{0};
    }
};

#endif // ZERO_ON_DESTRUCTION_HPP
