#ifndef MANUALLY_DESTRUCTED_HPP
#define MANUALLY_DESTRUCTED_HPP

#include <utility>
#include <memory>

namespace genex {

template<typename T>
class manually_destructed {
public:

    template<typename... Args>
    explicit manually_destructed(Args&&... args) :
        storage(std::forward<Args>(args)...)
    {}

    template<typename... Args>
    void emplace(Args&&... args) {
        ::new (&storage.object) T(std::forward<Args>(args)...);
    }

    void erase() {
        std::destroy_at(&storage.object);
    }

    operator T&() {
        return storage.object;
    }

    T * get_pointer() {
        return &storage.object;
    }

    T const * get_pointer() const {
        return &storage.object;
    }

private:
    union storage_type {
        T object;
        char dummy;

        template<typename... Args>
        explicit storage_type(Args&&... args)
            : object(std::forward<Args>(args)...)
        {}

        ~storage_type() {}
    } storage;
};

} // end namespace genex

#endif // MANUALLY_DESTRUCTED_HPP
