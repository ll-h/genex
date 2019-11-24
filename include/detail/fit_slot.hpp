#ifndef FIT_SLOT_HPP
#define FIT_SLOT_HPP

#include <type_traits>
#include <utility>
#include <cstddef>
#include <cstdint>
#include <algorithm>

namespace genex {

namespace detail {

template<std::size_t TypeIndex,
         typename Slot,
         typename SlotType = std::remove_reference_t<Slot>>
using slot_alternative_t = std::conditional_t<
    TypeIndex == 0,
    typename SlotType::free_index_type,
    typename SlotType::object_type>;

} // end detail

template<std::size_t TypeIndex, typename Slot>
detail::slot_alternative_t<TypeIndex, Slot> get(Slot&& slot);


namespace detail {

template<std::size_t TypeIndex>
constexpr void type_index_assert() {
    static_assert(TypeIndex < 2,
                  "This variant type is only intended to be used with "
                  "a gic_fit type");
}

struct packing_policy {};
// Use for faster single access to the held object but possibly less packing.
struct wide_slot : packing_policy {};
// Use for optimal packing but possibly slower single access to the held object.
struct packed_slot : packing_policy {};


template<typename T, typename FreeIndex, typename PackingPolicy = packed_slot>
class fit_slot
{
private:
    static_assert(std::is_base_of_v<packing_policy, PackingPolicy>);

    static constexpr std::size_t union_size =
            std::max(sizeof(T), sizeof(FreeIndex));

    struct wide_storage {
        alignas(alignof(T)) std::byte storage[union_size];
        uint8_t type_index{0};

        uint8_t index() const {
            return type_index;
        }

        void set_index(uint8_t idx) {
            type_index = idx;
        }
    };

    struct packed_storage {
        alignas(1) std::byte storage[union_size + 1];

        uint8_t& index_ref() & {
            return reinterpret_cast<uint8_t&>(storage[union_size]);
        }

        uint8_t const& index_ref() const& {
            return reinterpret_cast<uint8_t const&>(storage[union_size]);
        }

        packed_storage() {
            index_ref() = 0;
        }

        uint8_t index() const {
            return index_ref();
        }

        void set_index(uint8_t idx) {
            index_ref() = idx;
        }
    };

    using storage_type = std::conditional_t<
        std::is_same_v<PackingPolicy, packed_slot>,
        packed_storage,
        wide_storage>;

    storage_type storage;

    template<typename A>
    A const* reinterpreted_address() const& {
        return reinterpret_cast<A const*>(std::addressof(storage.storage));
    }

    template<typename A>
    A* reinterpreted_address() & {
        return reinterpret_cast<A*>(std::addressof(storage.storage));
    }

    T* object_address() & {
        return reinterpreted_address<T>();
    }


    template<std::size_t Index>
    void destroy_storage() {
        using held_type = slot_alternative_t<Index, fit_slot>;
        std::destroy_at(reinterpreted_address<held_type>());
    }


    template<std::size_t I, typename S>
    friend slot_alternative_t<I, S> genex::get(S&& slot);

public:

    using fit_slot_type = fit_slot;
    using free_index_type = FreeIndex;
    using object_type = T;

    template<typename... Args>
    fit_slot(std::in_place_index_t<1> in_place, Args&&... args)
    {
        (void)in_place;
        storage.set_index(1);
        ::new (object_address()) T(std::forward<Args>(args)...);
    }

    fit_slot() = default;
    fit_slot(fit_slot const& to_copy) {
        if(index() == to_copy.index()) {
            if(index() == 0) {
                reinterpreted_address<0>() = to_copy.reinterpreted_address<0>();
            }
            else {
                reinterpreted_address<1>() = to_copy.reinterpreted_address<1>();
            }
        }
        else {
            if(index() == 0) {
                destroy_storage<0>();
                ::new (object_address()) T(to_copy.reinterpreted_address<1>());
            }
            else {
                destroy_storage<1>();
                ::new (reinterpreted_address<0>())
                        FreeIndex(to_copy.reinterpreted_address<0>());
            }
            storage.set_index(to_copy.storage.index());
        }
    }

    ~fit_slot() {
        if(index() == 0) {
            destroy_storage<0>();
        }
        else {
            destroy_storage<1>();
        }
    }


    template<std::size_t TypeIndex, typename... Args>
    decltype(auto) emplace(Args&&... args)
    {
        type_index_assert<TypeIndex>();

        // gic_fit always calls emplace when the type needs to change,
        // so no need to test here the value of index()

        destroy_storage<(TypeIndex+1)%2>();
        storage.set_index(TypeIndex);

        using constructed_type = slot_alternative_t<TypeIndex, fit_slot>;
        auto* ptr = ::new (reinterpreted_address<constructed_type>())
              constructed_type(std::forward<Args>(args)...);
        return *ptr;
    }

    std::size_t index() const {
        return storage.index();
    }
};

} // namespace detail

template<std::size_t TypeIndex, typename Slot>
detail::slot_alternative_t<TypeIndex, Slot> get(Slot&& slot) {
    detail::type_index_assert<TypeIndex>();

    using returned_type = detail::slot_alternative_t<TypeIndex, Slot>;
    return *slot.template reinterpreted_address<returned_type>();
}

} // namespace genex

#endif // FIT_SLOT_HPP
