#ifndef GIC_CORE_ACCESS_HPP
#define GIC_CORE_ACCESS_HPP

#include <utility>
#include "perfect_backward.hpp"
#include "gic_base_forward_declaration.hpp"

namespace genex::detail {

// Eases CRTP implementation by :
// - enabling using private members of the derived class in the base class
// - enabling using static members of the derived class in the base class
// This trick is used in Boost.Iterator. More specifically, the class
// iterator_core_access defined in iterator_facade.hpp.
class gic_core_access {

    template<typename... Param>
    friend class gic_base;

    template<typename Derived, typename B, typename E>
    static decltype(auto) make_iterator(Derived& gic,
                                        B&& begin_getter,
                                        E&& end_getter)
    {
        return PERFECT_BACKWARD(Derived::make_iterator(
                                    gic,
                                    std::forward<B>(begin_getter),
                                    std::forward<E>(end_getter)));
    }
};

} // end namespace

#endif // GIC_CORE_ACCESS_HPP
