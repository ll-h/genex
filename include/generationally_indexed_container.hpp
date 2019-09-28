#ifndef GENERATIONALLY_INDEXED_CONTAINER_HPP
#define GENERATIONALLY_INDEXED_CONTAINER_HPP

#include <cstdint>
#include <utility>
#include <functional>
#include <algorithm>
#include <vector>
#include <iterator>
#include <type_traits>

#include "manually_destructed.hpp"
#include "element_validity_embedded_in_generation.hpp"
#include "gic_iterator.hpp"

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


template<typename T,
         template<class> class ObjectContainer = std::vector,
         typename Index = size_t,
         typename Generation = size_t,
         class IndexContainer = std::vector<Index>,
         class GenerationContainer = std::vector<Generation>>
class gic {
public:
    using key_type = key<T, Index, Generation>;
    using wrapped_type = manually_destructed<T>;
    using wrapped_object_constainer = ObjectContainer<wrapped_type>;
    using element_access_type = T*;
    using element_const_access_type = T const *;
    using iterator = x_iterator<
        typename GenerationContainer::iterator,
        typename wrapped_object_constainer::iterator>;
    using const_iterator = x_iterator<
        typename GenerationContainer::const_iterator,
        typename wrapped_object_constainer::const_iterator>;


    gic() {}

    ~gic() {
        // all living objects must be destroyed
        Index const max = generations.size();
        for(Index index = 0; index < max; ++index) {
            if(genex::is_valid(generations[index])) {
                objects[index].erase();
            }
        }
    }

    [[nodiscard]] element_access_type
    get(key_type const & k) {
        return internal_get(*this, k);
    }

    [[nodiscard]] element_const_access_type
    get(key_type const & k) const {
        return internal_get(*this, k);
    }

    [[nodiscard]] element_access_type
    operator [](key_type const & k) {
        return get(k);
    }

    [[nodiscard]] element_const_access_type
    operator [](key_type const & k) const {
        return get(k);
    }

    [[nodiscard]] element_access_type failed_get() {
        return nullptr;
    }

    [[nodiscard]] element_const_access_type failed_get() const {
        return nullptr;
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

    void remove(key_type const & k) {
        auto gen = k.get_generation();
        if(genex::is_valid(gen)) {
            auto idx = k.get_index();

            if(gen == generations[idx]) {
                unchecked_erasure(std::forward<Index>(idx));
            }
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
    GenerationContainer generations;
    IndexContainer free_indexes;

    void unchecked_erasure(Index&& idx) {
        ++generations[idx];
        objects[idx].erase();
        free_indexes.push_back(idx);
    }

    // This is clearly overkill, I just wanted to see if I could write
    // the logic of this function only once instead of once for const
    // this and once for non-const this
    template<class Self>
    friend auto internal_get(
            Self&& self,
            key_type const & k)
    {
        auto gen = k.get_generation();
        if(genex::is_valid(gen)) {
            auto idx = k.get_index();
            if(gen == self.generations[idx]) {
                return self.objects[idx].get_pointer();
            }
        }

        return self.failed_get();
    }
};

} // end namespace genex

template<bool IsConst,
         typename... GicArgs>
void swap(typename genex::gic<GicArgs...>::template x_iterator<IsConst>& it_a,
          typename genex::gic<GicArgs...>::template x_iterator<IsConst>& it_b)
{
    using std::swap;
    swap(*it_a, *it_b);
}

#endif // GENERATIONALLY_INDEXED_CONTAINER_HPP
