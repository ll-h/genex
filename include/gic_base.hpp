#ifndef GIC_BASE_HPP
#define GIC_BASE_HPP

#include <utility>

#include "key.hpp"
#include "detail/genex_crtp.hpp"
#include "detail/gic_base_forward_declaration.hpp"
#include "detail/gic_core_access.hpp"
#include "detail/iterator_utils.hpp"
#include "detail/perfect_backward.hpp"

namespace genex {

// Base class for generationally indexed containers
template<typename Derived,
         typename T,
         typename Key,
         typename GenerationContainer>
class gic_base : public detail::crtp_base<Derived> {
public:
    using value_type = T;
    using key_type = Key;
    static_assert (is_tagged_key_v<key_type, value_type>);

    using generation_type = typename key_type::generation_type;
    using element_access_type = value_type*;
    using element_const_access_type = value_type const *;

    [[nodiscard]] element_access_type
    get(key_type const & k) {
        return internal_get(*this, k);
    }

    [[nodiscard]] element_const_access_type
    get(key_type const & k) const {
        return internal_get(*this, k);
    }

    [[nodiscard]] element_access_type
    operator [](key_type const & k) {
        return get(k);
    }

    [[nodiscard]] element_const_access_type
    operator [](key_type const & k) const {
        return get(k);
    }

    bool is_present(key_type const &k) const {
        return k.get_generation() == generations[k.get_index()];
    }

    [[nodiscard]] element_access_type failed_get() {
        return nullptr;
    }

    [[nodiscard]] element_const_access_type failed_get() const {
        return nullptr;
    }


    decltype(auto) begin() {
        return PERFECT_BACKWARD(
            detail::gic_core_access::make_iterator(this->as_derived(),
                                                   detail::begin_getter_v,
                                                   detail::end_getter_v));
    }

    decltype(auto) cbegin() const {
        return PERFECT_BACKWARD(
            detail::gic_core_access::make_iterator(this->as_derived(),
                                                   detail::cbegin_getter_v,
                                                   detail::cend_getter_v));
    }

    decltype(auto) begin() const {
        return PERFECT_BACKWARD(cbegin());
    }

    decltype(auto) end() {
        return PERFECT_BACKWARD(
            detail::gic_core_access::make_iterator(this->as_derived(),
                                                   detail::end_getter_v,
                                                   detail::end_getter_v));
    }

    decltype(auto) cend() const {
        return PERFECT_BACKWARD(
            detail::gic_core_access::make_iterator(this->as_derived(),
                                                   detail::cend_getter_v,
                                                   detail::cend_getter_v));
    }

    decltype(auto) end() const {
        return PERFECT_BACKWARD(cend());
    }

protected:
    gic_base() = default;

    GenerationContainer generations;

private:

    // This is clearly overkill, I just wanted to see if I could write
    // the logic of this function only once instead of once for const
    // this and once for non-const this
    template<class Self>
    static decltype(auto) internal_get(
            Self&& self,
            key_type const & k)
    {
        if(self.as_derived().is_present(k)) {
            return detail::gic_core_access::unchecked_get(self.as_derived(),
                                                          k.get_index());
        }

        return self.failed_get();
    }
};

} // end namespace genex


#endif // GIC_BASE_HPP
