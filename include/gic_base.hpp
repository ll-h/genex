#ifndef GIC_BASE_HPP
#define GIC_BASE_HPP

#include <utility>

#include "key.hpp"
#include "genex_crtp.hpp"
#include "detail/gic_base_forward_declaration.hpp"

namespace genex {

// Base class for generationally indexed containers
template<typename Derived,
         typename T,
         typename Key,
         typename GenerationContainer>
class gic_base : public crtp_base<Derived> {
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

protected:
    gic_base() = default;

    GenerationContainer generations;

private:

    // This is clearly overkill, I just wanted to see if I could write
    // the logic of this function only once instead of once for const
    // this and once for non-const this
    template<class Self>
    friend decltype(auto) internal_get(
            Self&& self,
            key_type const & k)
    {
        if(self.as_derived().is_present(k)) {
            return self.as_derived().unchecked_get(k.get_index());
        }

        return self.failed_get();
    }
};

} // end namespace genex


#endif // GIC_BASE_HPP
