#ifndef GIC_KEY_HPP
#define GIC_KEY_HPP

#include <cstddef>
#include <utility>

namespace genex {

template<class Tag,
         typename Index = size_t,
         typename Generation = size_t,
         Generation InitialGeneration = 0>
class key {
public:
    key(Index&& index, Generation&& gen = InitialGeneration) :
        index(std::forward<Index>(index)),
        generation(std::forward<Generation>(gen))
    {}

    key(Index const & index, Generation const & gen) :
        index(index),
        generation(gen)
    {}

    Generation const & get_generation() const {
        return generation;
    }

    Index const & get_index() const {
        return index;
    }

private:
    Index index;

    // even for empty keys, odd for keys of living objects
    Generation generation;
};


} // end namespace genex

#endif // GIC_KEY_HPP
