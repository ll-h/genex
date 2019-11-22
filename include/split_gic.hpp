#ifndef SPLIT_GENERATIONALLY_INDEXED_CONTAINER_HPP
#define SPLIT_GENERATIONALLY_INDEXED_CONTAINER_HPP

#include <cstddef>
#include <utility>
#include <functional>
#include <algorithm>
#include <vector>
#include <iterator>
#include <type_traits>

#include "gic_with_generations.hpp"
#include "key.hpp"
#include "detail/manually_destructed.hpp"
#include "detail/element_validity_embedded_in_generation.hpp"
#include "detail/split_gic_iterator.hpp"
#include "detail/perfect_backward.hpp"
#include "detail/key_placeholding.hpp"

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
        public gic_with_generations<
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
    using parent_type =
        gic_with_generations<split_gic, T, Key, GenerationContainer>;

    // without this line, we can only refer to 'generations' with
    // 'this->generations' because the base class is templated.
    using parent_type::generations;

public:
    using key_type = typename parent_type::key_type;
    using index_type = typename key_type::index_type;
    using generation_type = typename key_type::generation_type;

    using wrapped_type = detail::manually_destructed<T>;
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
            if(genex::detail::is_valid(*gen_it)) {
                obj_it->erase();
            }
        }
    }

    template<typename... Args>
    [[nodiscard]] std::pair<key_type, T&> emplace_and_get(Args&&... args) {
        if (free_indexes.empty()) {
            key_type k{index_type{objects.size()},
                       generations.emplace_back()};
            auto& slot = objects.emplace_back(
                        detail::forward_arg_or_key<Args>(args, k)...);

            return {k, *slot};
        }
        else {
            auto idx = free_indexes.back();
            free_indexes.pop_back();
            key_type k{idx, ++generations[idx]};
            T& obj = objects[idx].emplace(
                        detail::forward_arg_or_key<Args>(args, k)...);

            return {k, obj};
        }
    }

    void remove(key_type const &k) {
        if(this->is_present(k)) {
            auto idx = k.get_index();
            unchecked_erasure(std::forward<index_type>(idx));
        }
    }

    void erase(index_type&& index) {
        if(genex::detail::is_valid(generations[index])) {
            unchecked_erasure(std::forward<index_type>(index));
        }
    }

private:
    ObjectContainer<wrapped_type> objects;
    IndexContainer free_indexes;

    void unchecked_erasure(index_type&& idx) {
        ++generations[idx];
        objects[idx].erase();
        free_indexes.push_back(idx);
    }


    friend class detail::gic_core_access;

    template<class Self>
    static decltype(auto) unchecked_get(Self& self, index_type const& idx) {
        return PERFECT_BACKWARD(self.objects[idx].get_pointer());
    }

    // Main implementation of the iterator.
    // The rest is used to retrieve its types (const and non-const).
    template <typename Self, typename BG, typename EG>
    static decltype(auto)
    make_iterator(Self& self, BG&& begin_getter, EG&& end_getter) {
        return PERFECT_BACKWARD(
            detail::make_split_gic_iterator(self.generations,
                                            self.objects,
                                            begin_getter,
                                            end_getter));
    }
};

} // end namespace genex

#endif // SPLIT_GENERATIONALLY_INDEXED_CONTAINER_HPP
