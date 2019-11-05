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

struct cbegin_getter {
    template<typename Container>
    decltype(auto) operator()(Container& cont) const {
        return PERFECT_BACKWARD(cont.cbegin());
    }
};

struct end_getter {
    template<typename Container>
    decltype(auto) operator()(Container& cont) const {
        return PERFECT_BACKWARD(cont.end());
    }
};

struct cend_getter {
    template<typename Container>
    decltype(auto) operator()(Container& cont) const {
        return PERFECT_BACKWARD(cont.cend());
    }
};

} // end namespace genex::detail

#endif // ITERATOR_UTILS_HPP
