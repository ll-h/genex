#ifndef GIC_KEY_HPP
#define GIC_KEY_HPP

#include <stdint.h>
#include <utility>

#include "concept_helpers.hpp"

namespace genex {

// This function exhaustively lists the required behaviors of a type that can be
// used as a key for genex constainers. This function is not meant to be called,
// it is meant to be read by implementers of such key types and to be used for
// implementing is_key.
template<typename Key>
auto key_requirements() {
    // public types
    using Index = typename Key::index_type;
    using Generation = typename Key::generation_type;

    // constructors
    auto constructor_by_cref = [] (Index const &i, Generation const &g) {
        Key k(i, g);
    };
    auto constructor_by_rvalref = [] (Index&& i, Generation&& g) {
        Key k(std::forward<Index>(i), std::forward<Generation>(g));
    };

    // public member functions
    auto public_member_functions = [] (Key const& k) {
        Index const& i = k.get_index();
        Generation const& g = k.get_generation();
    };


    // "use" unused variables
    (void)constructor_by_cref;
    (void)constructor_by_rvalref;
    (void)public_member_functions;

    // using "auto" as return type instead of just "void" forces the compiler to
    // look into this function and check if its body can be instanciated with T.
    // Otherwise, decltype(key_requirements(...)) would yeild the type known
    // from the signature of the function.
    return;
}

template<typename T, typename Enable = void>
struct is_key : std::false_type {};

template<typename T>
struct is_key<T, decltype(key_requirements<T>())>
    : std::true_type {};

template<typename T>
static constexpr bool is_key_v = is_key<T>::value;

// Default key implementation. It is used as default tmeplate parameter of genex
// containers.
template<class Tag,
         typename Index = uint32_t,
         typename Generation = uint32_t,
         Generation InitialGeneration = 0>
class key {
public:
    using index_type = Index;
    using generation_type = Generation;

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
