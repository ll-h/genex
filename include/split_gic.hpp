#ifndef SPLIT_GENERATIONALLY_INDEXED_CONTAINER_HPP
#define SPLIT_GENERATIONALLY_INDEXED_CONTAINER_HPP

#include <cstddef>
#include <utility>
#include <functional>
#include <algorithm>
#include <vector>
#include <iterator>
#include <type_traits>

#include "gic_base.hpp"
#include "key.hpp"
#include "manually_destructed.hpp"
#include "element_validity_embedded_in_generation.hpp"
#include "split_gic_iterator.hpp"

namespace genex {

// A generationnally indexed container where the objects, their generation and
// the indexes of freed objects are in separate containers and whether an object
// is free or not is determined by the generation.
template<typename T,
         template<class...> class ObjectContainer = std::vector,
         class Key = key<T>,
         class IndexContainer = std::vector<typename Key::index_type>,
         class GenerationContainer = std::vector<typename Key::generation_type>>
class split_gic :
        public gic_base<
            split_gic<
                T,
                ObjectContainer,
                Key,
                IndexContainer,
                GenerationContainer>,
            T,
            Key,
            GenerationContainer
        >
{
private:
    using parent_type = gic_base<split_gic, T, Key, GenerationContainer>;

    // without this line, we can only refer to 'generations' with
    // 'this->generations' because the base class is templated.
    using parent_type::generations;

public:
    using key_type = typename parent_type::key_type;
    using index_type = typename key_type::index_type;
    using generation_type = typename key_type::generation_type;

    using wrapped_type = manually_destructed<T>;
    using wrapped_object_container = ObjectContainer<wrapped_type>;

    using iterator = detail::split_gic_iterator<
        GenerationContainer,
        wrapped_object_container>;

    using const_iterator = detail::split_gic_iterator<
        GenerationContainer const,
        wrapped_object_container const>;

    split_gic() = default;

    ~split_gic() {
        // all living objects must be destroyed
        auto obj_it = objects.begin();
        auto gen_it = generations.cbegin();
        for(const auto gen_end = generations.cend();
            gen_it != gen_end;
            ++gen_it, ++obj_it)
        {
            if(genex::is_valid(*gen_it)) {
                obj_it->erase();
            }
        }
    }

    template<typename... Args>
    [[nodiscard]] key_type emplace(Args&&... args) {
        if (free_indexes.empty()) {
            key_type k{index_type{objects.size()}, generation_type{}};
            objects.emplace_back(std::forward<Args>(args)...);
            generations.push_back(k.get_generation());
            return k;
        }
        else {
            auto idx = free_indexes.back();
            auto & gen = generations[idx];
            ++gen;
            key_type k{idx, gen};
            objects[idx].emplace(std::forward<Args>(args)...);
            free_indexes.pop_back();

            return k;
        }
    }

    void remove(key_type const &k) {
        if(this->is_present(k)) {
            auto idx = k.get_index();
            unchecked_erasure(std::forward<index_type>(idx));
        }
    }

    void erase(index_type&& index) {
        if(genex::is_valid(generations[index])) {
            unchecked_erasure(std::forward<index_type>(index));
        }
    }

    iterator begin() {
        return detail::make_split_gic_iterator<detail::begin_getter,
                                               detail::end_getter>
                (generations, objects);
    }

    const_iterator cbegin() const {
        return detail::make_split_gic_iterator<detail::cbegin_getter,
                                               detail::cend_getter>
                (generations, objects);
    }

    const_iterator begin() const {
        return cbegin();
    }

    iterator end() {
        return detail::make_split_gic_iterator<detail::end_getter,
                                               detail::end_getter>
                (generations, objects);
    }

    const_iterator cend() const {
        return detail::make_split_gic_iterator<detail::cend_getter,
                                               detail::cend_getter>
                (generations, objects);
    }

    const_iterator end() const {
        return cend();
    }

    decltype(auto) unchecked_get(index_type const& idx) {
        return objects[idx].get_pointer();
    }

    decltype(auto) unchecked_get(index_type const& idx) const {
        return objects[idx].get_pointer();
    }

private:
    ObjectContainer<wrapped_type> objects;
    IndexContainer free_indexes;

    void unchecked_erasure(index_type&& idx) {
        ++generations[idx];
        objects[idx].erase();
        free_indexes.push_back(idx);
    }
};

} // end namespace genex

#endif // SPLIT_GENERATIONALLY_INDEXED_CONTAINER_HPP
