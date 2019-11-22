#ifndef FAST_ITERABLE_GIC_HPP
#define FAST_ITERABLE_GIC_HPP

#include <cstddef>
#include <utility>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <variant>

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "detail/gic_core_access.hpp"
#include "detail/perfect_backward.hpp"
#include "detail/key_placeholding.hpp"
#include "gic_with_generations.hpp"


namespace genex {

// A generationnally indexed container tuned to have fast iteration over its
// elements at the cost of a space overhead of one byte per element.
//
// Its free objects are used to store a singly-linked list of the indexes of
// the free objects. One byte per element is used to determin whether or not it
// is free. This byte is placed in an address adjacent to the object (or index),
// which means that iterating over the elements of the container only require
// a few more cache misses than iterating over a plain array of object.
template<typename T,
         template<class...> class ObjectContainer,
         class Key,
         class GenerationContainer,
         template<class...> class Variant = std::variant>
class gic_fit :
        public gic_with_generations<
            gic_fit<
                T,
                ObjectContainer,
                Key,
                GenerationContainer,
                Variant>,
            T,
            Key,
            GenerationContainer
        >
{
private:
    using parent_type =
        gic_with_generations<gic_fit, T, Key, GenerationContainer>;

    // without this line, we can only refer to 'generations' with
    // 'this->generations' because the base class is templated.
    using parent_type::generations;

public:
    using value_type = T;
    using reference = T&;
    using const_reference = T const&;
    using key_type = typename parent_type::key_type;
    using index_type = typename key_type::index_type;
    using generation_type = typename key_type::generation_type;

    using wrapped_type = Variant<index_type, T>;
    using wrapped_object_container = ObjectContainer<wrapped_type>;

private:
    wrapped_object_container objects;

    // head of the singly-linked list of free elements
    index_type free_head;
    index_type number_of_free_elements{0};

    void unchecked_erasure(index_type&& idx) {
        ++generations[idx];
        objects[idx].template emplace<0>(free_head);
        free_head = idx;
        ++number_of_free_elements;
    }

    // ===== CRTP overrides =====

    friend class detail::gic_core_access;

    template<class Self>
    static decltype(auto) unchecked_get(Self& self, index_type const& idx) {
        return PERFECT_BACKWARD(
                    std::addressof(std::get<1>(self.objects[idx])));
    }


    // ===== Iterator ====

    // filter
    struct is_slot_occupied {
        // making this function 'const' is necessary. It was HARD to find out.
        bool operator()(wrapped_type const& slot) const {
            return slot.index() == 1;
        }
    };

    // transform
    struct slot_unwrapper {
        reference operator()(wrapped_type & slot) const {
            return std::get<1>(slot);
        }

        const_reference operator()(wrapped_type const& slot) const {
            return std::get<1>(slot);
        }
    };

    // Main implementation of the iterator.
    // The rest is used to retrieve its types (const and non-const).
    template <typename ObjCont, typename BG, typename EG>
    static decltype(auto)
    make_gic_fit_iterator(ObjCont& container,
                          BG begin_getter,
                          EG end_getter)
    {
        return PERFECT_BACKWARD(
            boost::make_transform_iterator<slot_unwrapper>(
                boost::make_filter_iterator<is_slot_occupied>(
                    begin_getter(container),
                    end_getter(container))));
    }

    template <typename Self, typename BG, typename EG>
    static decltype(auto)
    make_iterator(Self& self, BG begin_getter, EG end_getter) {
        return PERFECT_BACKWARD(
            make_gic_fit_iterator(self.objects,
                                  begin_getter,
                                  end_getter));
    }

    template<bool IsConst>
    using conditionally_const_container = std::conditional_t<
        IsConst,
        const wrapped_object_container,
        wrapped_object_container>;

    template<bool IsConst>
    using conditionally_const_iterator = decltype(make_gic_fit_iterator(
        std::declval<conditionally_const_container<IsConst>&>(),
        detail::begin_getter_v,
        detail::end_getter_v));

public:
    using iterator = conditionally_const_iterator<false>;
    using const_iterator = conditionally_const_iterator<true>;


    // ===== Core functionalities =====

    ~gic_fit() = default;
    // destroys this->objects, which should call the destructor of each of its
    // elements, which are variant instances that call the destructor of the
    // index or object they hold.

    template<typename... Args>
    [[nodiscard]] std::pair<key_type, T&> emplace_and_get(Args&&... args) {
        if (number_of_free_elements != 0) {
            auto idx = free_head;
            key_type k{idx, ++generations[idx]};
            auto & slot = objects[idx];

            // by construction of this GIC, the variant alternative at free_head
            // is an index, but the call to this will make check regardless.
            // a custom variant with an "unchecked_get" would be preferable.
            free_head = std::get<0>(slot);
            --number_of_free_elements;

            T& emplaced_obj = slot.template emplace<1>(
                        detail::forward_arg_or_key<Args>(args, k)...);

            return {k, emplaced_obj};
        }
        else {
            key_type k{index_type{objects.size()}, generation_type{}};
            auto& slot = objects.emplace_back(
                        std::in_place_index<1>,
                        detail::forward_arg_or_key<Args>(args, k)...);

            generations.push_back(k.get_generation());
            return {k, std::get<1>(slot)};
        }
    }

    void remove(key_type const &k) {
        if(this->is_present(k)) {
            auto idx = k.get_index();
            unchecked_erasure(std::forward<index_type>(idx));
        }
    }
};

} // end namespace genex

#endif // FAST_ITERABLE_GIC_HPP
