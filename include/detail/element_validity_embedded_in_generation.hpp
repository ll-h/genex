#ifndef ELEMENT_VALIDITY_EMBEDDED_IN_GENERATION_HPP
#define ELEMENT_VALIDITY_EMBEDDED_IN_GENERATION_HPP

namespace genex::detail {

template<typename G>
bool is_valid(G&& generation) {
    return generation%2 == 0;
}

template<typename G>
bool is_valid(G const & generation) {
    return generation%2 == 0;
}

} // end namespace genex::detail

#endif // ELEMENT_VALIDITY_EMBEDDED_IN_GENERATION_HPP
