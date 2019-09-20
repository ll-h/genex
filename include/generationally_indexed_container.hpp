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

namespace genex {

template<typename G>
bool is_valid(G&& generation) {
    return generation%2 == 0;
}

template<typename G>
bool is_valid(G const & generation) {
    return generation%2 == 0;
}

template<typename Index = size_t, typename Generation = size_t>
class key {
public:
    key(Index&& index, Generation&& gen = 0) :
        index(std::forward<Index>(index)),
        generation(std::forward<Generation>(gen))
    {}

    key(Index const & index, Generation const & gen) :
        index(index),
        generation(gen)
    {}

    bool is_valid() const {
        return genex::is_valid(generation);
    }

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


namespace internal {
template<bool is_const,
         class T,
         class GenerationContainer,
         class WrappedObjectContainer>
struct sub_iterator_types;

template<class T,
         class GenerationContainer,
         class WrappedObjectContainer>
struct sub_iterator_types<
        true, T, GenerationContainer, WrappedObjectContainer>
{
    using gen_iter = typename GenerationContainer::const_iterator;
    using obj_iter = typename WrappedObjectContainer::const_iterator;

    using reference = T const&;
    using pointer = T const*;
};

template<class T,
         class GenerationContainer,
         class WrappedObjectContainer>
struct sub_iterator_types<
        false, T, GenerationContainer, WrappedObjectContainer>
{
    using gen_iter = typename GenerationContainer::iterator;
    using obj_iter = typename WrappedObjectContainer::iterator;

    using reference = T&;
    using pointer = T*;
};

} // end namespace internal

template<typename T,
         template<class> class ObjectContainer = std::vector,
         typename Index = size_t,
         typename Generation = size_t,
         class IndexContainer = std::vector<Index>,
         class GenerationContainer = std::vector<Generation>>
class gic {
public:
    using key_type = key<Index, Generation>;
    using wrapped_type = manually_destructed<T>;
    using element_access_type = T*;
    using element_const_access_type = T const *;

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
        if(k.is_valid()) {
            auto idx = k.get_index();

            if(k.get_generation() == generations[idx]) {
                unchecked_erasure(std::forward<Index>(idx));
            }
        }
    }

    void erase(Index&& index) {
        if(genex::is_valid(generations[index])) {
            unchecked_erasure(std::forward<Index>(index));
        }
    }

    template<bool is_const = false>
    class x_iterator {
    private:
        using sub_iterator_types = internal::sub_iterator_types<
            is_const, T, GenerationContainer, ObjectContainer<wrapped_type>>;
        using gen_iter = typename sub_iterator_types::gen_iter;
        using obj_iter = typename sub_iterator_types::obj_iter;

        gen_iter gen_it;
        obj_iter obj_it;
    public:
        // Iterator traits
        using difference_type =
            typename std::iterator_traits<obj_iter>::difference_type;
        using value_type = T;
        using reference = typename sub_iterator_types::reference;
        using pointer = typename sub_iterator_types::pointer;
        using iterator_category = std::conditional_t<
            std::is_base_of_v<
                typename gen_iter::iterator_category,
                typename obj_iter::iterator_category>,
            typename gen_iter::iterator_category,
            typename obj_iter::iterator_category
        >;

        x_iterator(gen_iter&& gi, obj_iter&& oi)
            : gen_it(std::forward<gen_iter>(gi)),
              obj_it(std::forward<obj_iter>(oi))
        {}

    };

    using iterator = x_iterator<false>;
    using const_iterator = x_iterator<true>;

    iterator begin() {
        return {generations.begin(), objects.begin()};
    }

    const_iterator begin() const {
        return {generations.cbegin(), objects.cbegin()};
    }

    const_iterator cbegin() const {
        return {generations.cbegin(), objects.cbegin()};
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
        if (k.is_valid()) {
            auto idx = k.get_index();
            if(k.get_generation() == self.generations[idx]) {
                return self.objects[idx].get_pointer();
            }
        }

        return self.failed_get();
    }
};

} // end namespace genex

#endif // GENERATIONALLY_INDEXED_CONTAINER_HPP
