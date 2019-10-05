#ifndef CONCEPTS_HELPERS_HPP
#define CONCEPTS_HELPERS_HPP

#include <type_traits>

namespace genex {

// like std::declval but with no static assertions in the body to prevent its
// actual call. Suffice it to say, unsafe_declval should not be called.
template<typename T>
std::add_rvalue_reference_t<T> unsafe_declval() {
    return static_cast<std::add_rvalue_reference_t<T>>(
        *static_cast<std::add_pointer_t<T>>(nullptr));
}


}

#endif // CONCEPTS_HELPERS_HPP
