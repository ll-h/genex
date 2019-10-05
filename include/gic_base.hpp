#ifndef GIC_BASE_HPP
#define GIC_BASE_HPP

#include <utility>

#include "key.hpp"
#include "genex_crtp.hpp"

namespace genex {

// Base class for generationally indexed containers
template<class Derived,
         typename T,
         typename Index,
         typename GenerationContainer>
class gic_base : public crtp_base<Derived> {
public:
    using value_type = T;
    using generation_type = typename GenerationContainer::value_type;
    using key_type = key<value_type, Index, generation_type>;
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
            return get_access_to_element_at(self.as_derived(), k.get_index());
        }

        return self.failed_get();
    }
};

} // end namespace genex


#endif // GIC_BASE_HPP
