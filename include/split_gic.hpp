#ifndef GENERATIONALLY_INDEXED_CONTAINER_HPP
#define GENERATIONALLY_INDEXED_CONTAINER_HPP

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
#include "embedded_gen_iterator.hpp"

namespace genex {

// A generationnally indexed container where the objects, their generation and
// the indexes of freed objects are in separate containers and whether an object
// is free or not is determined by the generation.
template<typename T,
         template<class> class ObjectContainer = std::vector,
         typename Index = size_t,
         typename Generation = size_t,
         class IndexContainer = std::vector<Index>,
         class GenerationContainer = std::vector<Generation>>
class split_gic :
        public gic_base<
            split_gic<
                T,
                ObjectContainer,
                Index,
                Generation,
                IndexContainer,
                GenerationContainer>,
            T,
            Index,
            GenerationContainer
        >
{
private:
    using parent_type = gic_base<split_gic, T, Index, GenerationContainer>;
    using parent_type::generations;

public:
    using key_type = typename parent_type::key_type;
    using wrapped_type = manually_destructed<T>;
    using wrapped_object_constainer = ObjectContainer<wrapped_type>;
    using iterator = embedded_gen_iterator<
        typename GenerationContainer::iterator,
        typename wrapped_object_constainer::iterator>;
    using const_iterator = embedded_gen_iterator<
        typename GenerationContainer::const_iterator,
        typename wrapped_object_constainer::const_iterator>;


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
            key_type k(objects.size());
            objects.emplace_back(std::forward<Args>(args)...);
            generations.push_back(k.get_generation());
            return k;
        }
        else {
            auto idx = free_indexes.back();
            auto & gen = generations[idx];
            ++gen;
            key_type k(idx, gen);
            objects[idx].emplace(std::forward<Args>(args)...);
            free_indexes.pop_back();

            return k;
        }
    }

    bool is_present(key_type const &k) const {
        return k.get_generation() == generations[k.get_index()];
    }

    void remove(key_type const &k) {
        if(is_present(k)) {
            auto idx = k.get_index();
            unchecked_erasure(std::forward<Index>(idx));
        }
    }

    void erase(Index&& index) {
        if(genex::is_valid(generations[index])) {
            unchecked_erasure(std::forward<Index>(index));
        }
    }

    iterator begin() {
        return {
            generations.begin(),
            generations.end(),
            objects.begin()
        };
    }

    const_iterator cbegin() const {
        return {
            generations.cbegin(),
            generations.cend(),
            objects.cbegin()
        };
    }

    const_iterator begin() const {
        return cbegin();
    }

private:
    ObjectContainer<wrapped_type> objects;
    IndexContainer free_indexes;

    void unchecked_erasure(Index&& idx) {
        ++generations[idx];
        objects[idx].erase();
        free_indexes.push_back(idx);
    }

    friend parent_type;

    // templating on Self and using a "hidden friend" allows the body to be
    // written only once instead of once for each of the "this" alternatives
    // which are {&, const &, &&, const &&}. "const &&" is relevant because
    // it catches the mistake of calling std::cref on a temporary object.
    template<class Self>
    friend decltype(auto) get_access_to_element_at(
            Self&& self,
            const Index& idx)
    {
        return self.objects[idx].get_pointer();
    }
};

} // end namespace genex

#endif // GENERATIONALLY_INDEXED_CONTAINER_HPP
