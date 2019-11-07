#ifndef GIC_CORE_ACCESS_HPP
#define GIC_CORE_ACCESS_HPP

#include <utility>
#include "perfect_backward.hpp"
#include "gic_base_forward_declaration.hpp"
#include "iterator_utils.hpp"

namespace genex::detail {

// Eases CRTP implementation by :
// - enabling using private members of the derived class in the base class
// - enabling using static members of the derived class in the base class
// This trick is used in Boost.Iterator. More specifically, the class
// iterator_core_access defined in iterator_facade.hpp.
class gic_core_access {

    template<typename Derived,
             typename T,
             typename Key,
             typename GC>
    friend class ::genex::gic_base;


    template<class Derived>
    static decltype(auto)
    unchecked_get(Derived& gic, typename Derived::index_type const& idx)
    {
        return PERFECT_BACKWARD(Derived::unchecked_get(gic, idx));
    }


    template<typename Derived, typename B, typename E>
    static decltype(auto) make_iterator(Derived& gic,
                                        B&& begin,
                                        E&& end)
    {
        return PERFECT_BACKWARD(Derived::make_iterator(
                                    gic,
                                    std::forward<B>(begin),
                                    std::forward<E>(end)));
    }

    template<typename Derived>
    using iterator = decltype(make_iterator(
        std::declval<Derived&>(),
        begin_getter{},
        end_getter{}));

    template<typename Derived>
    using const_iterator = decltype(make_iterator(
        std::declval<Derived&>(),
        cbegin_getter{},
        cend_getter{}));
};

} // end namespace

#endif // GIC_CORE_ACCESS_HPP
