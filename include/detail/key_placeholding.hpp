#ifndef GENEX_KEY_PLACEHOLDING_HPP
#define GENEX_KEY_PLACEHOLDING_HPP

#include <type_traits>
#include <utility>
#include "perfect_backward.hpp"

namespace genex {

namespace detail {

struct key_placeholder_t {};
constexpr key_placeholder_t key_placeholder;

template<typename Arg, typename Key>
decltype(auto) forward_arg_or_key(Arg&& arg, Key key) {
    if constexpr (std::is_same_v<std::remove_reference_t<Arg> const,
                                 key_placeholder_t const>) {
        return key;
    }
    else {
        return PERFECT_BACKWARD(std::forward<Arg>(arg));
    }
}

} // end namespace detail

using detail::key_placeholder;

} // end namespace genex

#endif // GENEX_KEY_PLACEHOLDING_HPP
