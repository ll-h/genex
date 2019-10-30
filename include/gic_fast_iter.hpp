#ifndef GENERATIONALLY_INDEXED_CONTAINER_HPP
#define GENERATIONALLY_INDEXED_CONTAINER_HPP

#include <cstddef>
#include <utility>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <variant>

#include "gic_base.hpp"

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
class gic_fast_iter :
        public gic_base<
            gic_fast_iter<
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
    using parent_type = gic_base<gic_fast_iter, T, Key, GenerationContainer>;

    // without this line, we can only refer to 'generations' with
    // 'this->generations' because the base class is templated.
    using parent_type::generations;

public:
    using key_type = typename parent_type::key_type;
    using index_type = typename key_type::index_type;
    using generation_type = typename key_type::generation_type;

    using wrapped_type = Variant<index_type, T>;
    using wrapped_object_container = ObjectContainer<wrapped_type>;

    using iterator = typename wrapped_object_container::iterator;
    using const_iterator = typename wrapped_object_container::const_iterator;

    ~gic_fast_iter() = default;
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

    iterator begin() {
        return objects.begin();
    }

    const_iterator cbegin() const {
        return objects.cbegin();
    }

    const_iterator begin() const {
        return cbegin();
    }

    iterator end() {
        return objects.end();
    }

    const_iterator cend() const {
        return objects.cend();
    }

    const_iterator end() const {
        return cend();
    }

    decltype(auto) unchecked_get(index_type const& idx) {
        return std::addressof(std::get<1>(objects[idx]));
    }

    decltype(auto) unchecked_get(index_type const& idx) const {
        return std::addressof(std::get<1>(objects[idx]));
    }

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
};

} // end namespace genex

#endif // GENERATIONALLY_INDEXED_CONTAINER_HPP