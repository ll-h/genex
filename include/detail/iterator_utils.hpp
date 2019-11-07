#ifndef ITERATOR_UTILS_HPP
#define ITERATOR_UTILS_HPP

#include "perfect_backward.hpp"

namespace genex::detail {

struct begin_getter {
    template<typename Container>
    decltype(auto) operator()(Container& cont) const {
        return PERFECT_BACKWARD(cont.begin());
    }
};

constexpr begin_getter begin_getter_v;


struct cbegin_getter {
    template<typename Container>
    decltype(auto) operator()(Container& cont) const {
        return PERFECT_BACKWARD(cont.cbegin());
    }
};

constexpr cbegin_getter cbegin_getter_v;


struct end_getter {
    template<typename Container>
    decltype(auto) operator()(Container& cont) const {
        return PERFECT_BACKWARD(cont.end());
    }
};

constexpr end_getter end_getter_v;


struct cend_getter {
    template<typename Container>
    decltype(auto) operator()(Container& cont) const {
        return PERFECT_BACKWARD(cont.cend());
    }
};

constexpr cend_getter cend_getter_v;


} // end namespace genex::detail

#endif // ITERATOR_UTILS_HPP
