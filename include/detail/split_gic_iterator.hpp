#ifndef GIC_ITERATOR_HPP
#define GIC_ITERATOR_HPP

#include <functional>
#include <type_traits>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include "element_validity_embedded_in_generation.hpp"
#include "perfect_backward.hpp"
#include "detail/iterator_utils.hpp"

namespace genex::detail {

// The iterator will be constructed as follows :
//     zip(generation_iterator, object_iterator)
//         | filter<valid_generation>
//         | transform<get_object>
//
// I won't use lambdas because they can't be used in unevaluated contexts.

constexpr int generation_tuple_index = 0;
constexpr int object_tuple_index = 1;

struct valid_generation {
    template<typename RefPair>
    bool operator()(RefPair&& zipped) const {
        return genex::detail::is_valid(
                    zipped.template get<generation_tuple_index>());
    }
};

struct get_object {
    template<typename RefPair>
    decltype(auto) operator()(RefPair&& zipped) const {
        return PERFECT_BACKWARD(*zipped.template get<object_tuple_index>());
    }
};

template<typename IteratorGetter,
         typename GenerationContainer,
         typename ObjectContainer>
decltype(auto) make_zipped(GenerationContainer& gens,
                           ObjectContainer& objs)
{
    return PERFECT_BACKWARD(
                boost::make_zip_iterator(
                    boost::make_tuple(
                        std::invoke(IteratorGetter{}, gens),
                        std::invoke(IteratorGetter{}, objs))));
}


template<typename BeginGetter,
         typename EndGetter,
         typename GenerationContainer,
         typename ObjectContainer>
decltype(auto) make_split_gic_iterator(GenerationContainer& gens,
                                       ObjectContainer& objs,
                                       BeginGetter&& = BeginGetter{},
                                       EndGetter&& = EndGetter{})
{
    return PERFECT_BACKWARD(
        boost::make_transform_iterator<get_object>(
            boost::make_filter_iterator<valid_generation>(
                make_zipped<BeginGetter>(gens, objs),
                make_zipped<EndGetter>(gens, objs))));
}

template<class GenerationContainer, class ObjectContainer>
using split_gic_iterator =
    decltype(make_split_gic_iterator<begin_getter, end_getter>(
        std::declval<GenerationContainer&>(),
        std::declval<ObjectContainer&>()));

} // namespace genex::detail

#endif // GIC_ITERATOR_HPP
