#ifndef GIC_ITERATOR_HPP
#define GIC_ITERATOR_HPP

#include "element_validity_embedded_in_generation.hpp"
#include "manually_destructed.hpp"

namespace genex {

template<class GenerationIterator, class ObjectIterator>
class x_iterator {
private:
    static constexpr bool is_const_iterator_v =
        std::is_const_v<
            std::remove_reference_t<
                typename ObjectIterator::reference>>;

    using gen_iter = GenerationIterator;
    using obj_iter = ObjectIterator;

    gen_iter gen_it;
    const gen_iter end_gen_it;
    obj_iter obj_it;

    static constexpr bool assume_current_is_allocated = true;
    static constexpr bool do_not_assume_current_is_allocated = false;

    template<bool assume_current_is_allocated_v>
    void advance_to_next_allocated() {
        auto iteration_should_continue = [this] () {
            return (gen_it != end_gen_it) && !genex::is_valid(*gen_it);
        };

        if constexpr (!assume_current_is_allocated_v) {
            if (!iteration_should_continue())
                return;
        }
        do {
            ++gen_it;
            ++obj_it;
        } while(iteration_should_continue());
    }

public:
    // ==== Iterator traits ====

    using difference_type =
        typename std::iterator_traits<obj_iter>::difference_type;

    using value_type = typename ObjectIterator::value_type::value_type;

    using reference = std::add_lvalue_reference_t<
        std::conditional_t<
            is_const_iterator_v,
            std::add_const_t<value_type>,
            value_type
        >
    >;

    using pointer = std::add_pointer_t<
        std::conditional_t<
            is_const_iterator_v,
            std::add_const_t<value_type>,
            value_type
        >
    >;

    using iterator_category = std::conditional_t<
        std::is_base_of_v<
            typename gen_iter::iterator_category,
            typename obj_iter::iterator_category>,
        typename gen_iter::iterator_category,
        typename obj_iter::iterator_category
    >;

    // custom constructor
    x_iterator(gen_iter&& gi, gen_iter&& end_gi, obj_iter&& oi)
        : gen_it(std::forward<gen_iter>(gi)),
          end_gen_it(std::forward<gen_iter>(end_gi)),
          obj_it(std::forward<obj_iter>(oi))
    {
        // initialize the iterator to an allocated object
        advance_to_next_allocated<do_not_assume_current_is_allocated>();
    }

    // ==== Iterator concept functions ====

    x_iterator(x_iterator const &it) = default;

    x_iterator& operator=(x_iterator const &it) = default;

    x_iterator& operator++() {
        advance_to_next_allocated<assume_current_is_allocated>();
        return *this;
    }

    reference operator*() {
        return **obj_it;
    }
};

} // end namespace genex

template<class GIter, class OIter>
void swap(genex::x_iterator<GIter, OIter>& it_a,
          genex::x_iterator<GIter, OIter>& it_b)
noexcept(std::is_nothrow_swappable_v<
            typename genex::x_iterator<GIter, OIter>::value_type>)
{
    using std::swap;
    swap(*it_a, *it_b);
}

#endif // GIC_ITERATOR_HPP
