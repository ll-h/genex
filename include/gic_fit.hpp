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
#include "gic_base.hpp"
#include "perfect_backward.hpp"


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
        public gic_base<
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
    using parent_type = gic_base<gic_fit, T, Key, GenerationContainer>;

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

    friend class detail::gic_core_access;

    // Main implementation of the iterator.
    // The rest is used to retrieve its types (const and non-const).
    template <typename Self, typename BG, typename EG>
    static decltype(auto)
    make_iterator(Self &self, BG&& begin_getter, EG&& end_getter) {
        return PERFECT_BACKWARD(
            boost::make_transform_iterator<slot_unwrapper>(
                boost::make_filter_iterator<is_slot_occupied>(
                    begin_getter(self.objects),
                    end_getter(self.objects))));
    }

    template <typename Iter>
    static decltype(auto) make_end_iterator(Iter end_it) {
        return PERFECT_BACKWARD(make_iterator(end_it, end_it));
    }

    template<bool IsConst>
    using conditionally_const_base_iterator = std::conditional_t<
        IsConst,
        typename wrapped_object_container::const_iterator,
        typename wrapped_object_container::iterator
    >;

    template<bool IsConst>
    using conditionally_const_iterator = decltype(make_end_iterator(
        std::declval<conditionally_const_base_iterator<IsConst>>()));

public:
    using iterator = conditionally_const_iterator<false>;
    using const_iterator = conditionally_const_iterator<true>;

    iterator begin() {
        return make_iterator(objects.begin(),
                             objects.end());
    }

    const_iterator cbegin() const {
        return make_iterator(objects.cbegin(),
                             objects.cend());
    }

    const_iterator begin() const {
        return cbegin();
    }

    iterator end() {
        return make_end_iterator(objects.end());
    }

    const_iterator cend() const {
        return make_end_iterator(objects.cend());
    }

    const_iterator end() const {
        return cend();
    }


    // ===== Core functionalities =====

    ~gic_fit() = default;
    // destroys this->objects, which should call the destructor of each of its
    // elements, which are variant instances that call the destructor of the
    // index or object they hold.

    template<typename... Args>
    [[nodiscard]] key_type emplace(Args&&... args) {
        if (number_of_free_elements != 0) {
            auto idx = free_head;
            auto & obj = objects[idx];

            // by construction of this GIC, the variant alternative at free_head
            // is an index, but the call to this will make check regardless.
            // a custom variant with an "unchecked_get" would be preferable.
            free_head = std::get<0>(obj);
            obj.template emplace<1>(std::forward<Args>(args)...);

            --number_of_free_elements;

            auto & gen = generations[idx];
            ++gen;
            return {idx, gen};
        }
        else {
            key_type k{index_type{objects.size()}, generation_type{}};
            objects.emplace_back(std::in_place_index<1>,
                                 std::forward<Args>(args)...);
            generations.push_back(k.get_generation());
            return k;
        }
    }

    void remove(key_type const &k) {
        if(this->is_present(k)) {
            auto idx = k.get_index();
            unchecked_erasure(std::forward<index_type>(idx));
        }
    }


    // ===== CRTP overrides =====

    decltype(auto) unchecked_get(index_type const& idx) {
        return std::addressof(std::get<1>(objects[idx]));
    }

    decltype(auto) unchecked_get(index_type const& idx) const {
        return std::addressof(std::get<1>(objects[idx]));
    }
};

} // end namespace genex

#endif // FAST_ITERABLE_GIC_HPP
