#ifndef GIC_WITH_GENERATIONS_HPP
#define GIC_WITH_GENERATIONS_HPP

#include <utility>

#include "gic_base.hpp"

namespace genex {

// A base class for generationally indexed containers that have a separate
// container of the generations
template<typename Derived,
         typename T,
         typename Key,
         typename GenerationContainer>
class gic_with_generations : public gic_base<Derived, T, Key> {
public:
    bool is_present(Key const &k) const {
        return k.get_generation() == generations[k.get_index()];
    }

protected:
    gic_with_generations() = default;

    GenerationContainer generations;
};

} // end namespace genex


#endif // GIC_WITH_GENERATIONS_HPP
