#ifndef MANUALLY_DESTRUCTED_HPP
#define MANUALLY_DESTRUCTED_HPP

#include <utility>
#include <memory>
#include <type_traits>

namespace genex {

template<typename T, typename WhenDestroyed = char>
class manually_destructed {
public:

    using value_type = T;

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

    T * get_pointer() {
        return std::addressof(storage.object);
    }

    T const * get_pointer() const {
        return std::addressof(storage.object);
    }

    template<class Self,
             typename std::enable_if_t<
                 std::is_same_v<
                     std::remove_const_t<std::remove_reference_t<Self>>,
                     manually_destructed<T>
                 >, int> = 0>
    friend decltype(auto) operator*(Self&& self) {
        auto& ret = std::forward<Self>(self).storage.object;
        return ret;
    }

    WhenDestroyed& destroyed_space() {
        return storage.when_destroyed;
    }

private:
    // This union is what makes this type possible: managing its content is
    // done manually, by definition of a union, and this includes the
    // destruction. Therefore, when the instance of storage_type is destroyed,
    // nothing more than a call to its destructor is done, and since it is
    // empty, nothing happens. Without wrapping the object in this union, even
    // with an empty destructor of manually_destructed, the destructor of the
    // object would have been called.
    union storage_type {
        T object;
        WhenDestroyed when_destroyed;

        template<typename... Args>
        explicit storage_type(Args&&... args)
            : object(std::forward<Args>(args)...)
        {}

        ~storage_type() {}
    } storage;
};

} // end namespace genex

#endif // MANUALLY_DESTRUCTED_HPP
