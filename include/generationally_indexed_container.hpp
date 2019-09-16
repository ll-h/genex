#ifndef GENERATIONALLY_INDEXED_CONTAINER_HPP
#define GENERATIONALLY_INDEXED_CONTAINER_HPP

#include <cstdint>
#include <utility>
#include <optional>
#include <vector>

namespace genex {

template<typename G>
bool is_valid(G&& generation) {
    return generation%2 == 0;
}

template<typename Index = size_t, typename Generation = size_t>
class key {
public:
    key(Index&& index, Generation&& gen = 0) :
        index(std::forward<Index>(index)),
        generation(std::forward<Generation>(gen))
    {}

    bool is_valid() const {
        return genex::is_valid(std::forward<Generation>(generation));
    }

    void increase_generation() {
        ++generation;
    }

    Index const & get_index() {
        return index;
    }

private:
    Index index;

    // even for empty keys
    Generation generation;
};

template<typename T>
class manual_destruction_wrapper {
public:
    ~manual_destruction_wrapper() {
        // object is not destroyed
    }

    template<typename... Args>
    manual_destruction_wrapper(Args&&... args) :
        storage(std::forward<Args>(args)...)
    {}

    template<typename... Args>
    void emplace(Args&&... args) {
        ::new (&storage.object) T(std::forward<Args>(args)...);
    }

    void erase() {
        storage.object.~T();
    }

    operator T&() {
        return storage.object;
    }

private:
    union storage_type {
        T object;
        char dummy;

        template<typename... Args>
        storage_type(Args&&... args)
            : object(std::forward<Args>(args)...)
        {}

        ~storage_type() {}
    } storage;
};

template<typename T,
         template<class> class ObjectContainer = std::vector,
         typename Index = size_t,
         typename Generation = size_t,
         class IndexContainer = std::vector<Index>,
         class GenerationContainer = std::vector<Generation>>
class gic {
public:
    using key_type = key<Index, Generation>;

    gic() {}

    T* get(key_type const & k) {
        if (k.is_valid()) {
            auto idx = k.get_index();
            if(k.generation == generations[idx]) {
                return &objects[idx];
            }
        }

        return nullptr;
    }

    template<typename... Args>
    key_type emplace(Args&&... args) {
        if (free_indexes.empty()) {
            key_type k(objects.size());
            objects.emplace(std::forward<Args>(args)...);
            generations.push_back(k.get_generation());
            return k;
        }
        else {
            auto idx = free_indexes.back();
            key_type k(idx, generations[idx] + 1);
            objects[idx].emplace(std::forward<Args>(args)...);
        }
    }

    void remove(key_type const & k) {
        if(k.is_valid()) {
            auto idx = k.get_index();

            if(k.generation == generations[idx]) {
                unchecked_erasure(std::forward<Index>(idx));
            }
        }
    }

    void erase(Index&& index) {
        if(genex::is_valid(generations[index])) {
            unchecked_erasure(std::forward<Index>(index));
        }
    }

private:
    ObjectContainer<manual_destruction_wrapper<T>> objects;
    GenerationContainer generations;
    IndexContainer free_indexes;

    void unchecked_erasure(Index&& idx) {
        ++generations[idx];
        objects[idx].erase();
        free_indexes.push_back(idx);
    }
};

} // end namespace genex

#endif // GENERATIONALLY_INDEXED_CONTAINER_HPP
