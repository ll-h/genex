#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include <utility>
#include <algorithm>
#include <type_traits>
#include "generic_test_definitions.hpp"
using namespace boost::unit_test;


// ===== Iterator concept =====

BOOST_FIXTURE_TEST_CASE( deref_first_allocated, GicWithOneElementFixture ) {
    auto it = container.begin();
    BOOST_TEST(*it == NON_ZERO_VAL);
}

BOOST_FIXTURE_TEST_CASE( deref_implicit_const, GicWithOneElementFixture ) {
    auto it = std::as_const(container).begin();
    BOOST_TEST(*it == NON_ZERO_VAL);
}

BOOST_FIXTURE_TEST_CASE( deref_explicit_const, GicWithOneElementFixture ) {
    auto it = container.cbegin();
    BOOST_TEST(*it == NON_ZERO_VAL);
}

BOOST_FIXTURE_TEST_CASE( deref_first_free, GicFixture ) {
    auto free_key = container.emplace(NON_ZERO_VAL_1);
    (void)container.emplace(NON_ZERO_VAL_2);
    container.remove(free_key);
    auto it = container.begin();

    BOOST_TEST(*it == NON_ZERO_VAL_2);
}

BOOST_FIXTURE_TEST_CASE( iter_swap, GicFixture ) {
    auto key_a = container.emplace(NON_ZERO_VAL_1);
    auto key_b = container.emplace(NON_ZERO_VAL_2);

    auto ita = container.begin();
    auto itb = container.begin();
    ++itb;

    std::iter_swap(ita, itb);

    BOOST_TEST(*ita == NON_ZERO_VAL_2);
    BOOST_TEST(*itb == NON_ZERO_VAL_1);
    BOOST_TEST(*container[key_a] == NON_ZERO_VAL_2);
    BOOST_TEST(*container[key_b] == NON_ZERO_VAL_1);
}


// ===== Input Iterator concept =====

#ifndef ASSERT_TRUE
#define ASSERT_TRUE(x) BOOST_TEST(!!(x))
#endif

BOOST_FIXTURE_TEST_CASE( iterator_equality_on_empty, GicFixture ) {
    ASSERT_TRUE(container.begin() == container.end());
    ASSERT_TRUE(container.cbegin() == container.cend());
    ASSERT_TRUE(container.cend() == container.begin());

    ASSERT_TRUE(container.begin() == container.begin());
    ASSERT_TRUE(container.begin() == container.cbegin());

    auto const& cc = container;
    ASSERT_TRUE(cc.begin() == cc.end());
}

BOOST_FIXTURE_TEST_CASE( iterator_inequality, GicWithOneElementFixture ) {
    ASSERT_TRUE(container.begin() != container.end());
    ASSERT_TRUE(container.cbegin() != container.cend());
}

BOOST_FIXTURE_TEST_CASE( iterator_incr_to_end, GicWithOneElementFixture ) {
    auto it = container.cbegin();
    ASSERT_TRUE(it != container.cend());
    ++it;
    ASSERT_TRUE(it == container.cend());
}

BOOST_FIXTURE_TEST_CASE( iterator_incr_skips_free, GicFixture ) {
    (void)container.emplace(NON_ZERO_VAL_1);
    auto free_key = container.emplace(NON_ZERO_VAL);
    (void)container.emplace(NON_ZERO_VAL_2);
    container.remove(free_key);

    auto it = container.cbegin();
    ASSERT_TRUE(it != container.cend());

    BOOST_TEST(*it++ == NON_ZERO_VAL_1);

    ASSERT_TRUE(it != container.cend());
    BOOST_TEST(*it == NON_ZERO_VAL_2);
}


// ===== Output Iterator concept =====

template<typename T>
constexpr bool is_iter_writeable =
        std::is_assignable_v<decltype(*std::declval<T>()), int>;

BOOST_FIXTURE_TEST_CASE( iterator_write, GicWithOneElementFixture ) {
    auto it = container.begin();
    static_assert (is_iter_writeable<decltype(it)>);

    *it = NON_ZERO_VAL_2;
    ASSERT_TRUE(*container[key] == NON_ZERO_VAL_2);
}

BOOST_FIXTURE_TEST_CASE( const_iterator_is_not_writable, GicFixture ) {
    auto const& cc = container;
    static_assert (!is_iter_writeable<decltype(container.cbegin())>);
    static_assert (!is_iter_writeable<decltype(cc.begin())>);
}


// ===== Forward Iterator concept =====

BOOST_FIXTURE_TEST_CASE( iterator_singularity, GicFixture ) {
    using iter_t = decltype(container)::iterator;
    using const_iter_t = decltype(container)::const_iterator;

    // also checks default-constructibility
    ASSERT_TRUE(iter_t{} == container.end());
    ASSERT_TRUE(const_iter_t{} == container.cend());
}


template<typename Iter,
         // type aliases :
         typename ValType = typename std::iterator_traits<Iter>::value_type,
         typename RefType = typename std::iterator_traits<Iter>::reference>
using forward_iter_type_requirements = std::conjunction<
    std::is_same<
        RefType,
        std::conditional_t<
            is_iter_writeable<Iter>,
            ValType&,
            ValType const&
        >
    >,
    std::is_same<
        decltype(std::declval<Iter&>()++),
        Iter>,
    std::is_same<
        decltype(*std::declval<Iter&>()++),
        RefType
    >
>;

template<typename Iter>
constexpr bool are_forward_iter_type_requirements_ok =
        forward_iter_type_requirements<Iter>::value;

BOOST_AUTO_TEST_CASE( iterator_deref_types ) {
    using iter = gic_derived<int>::iterator;
    using citer = gic_derived<int>::const_iterator;

    static_assert(are_forward_iter_type_requirements_ok<iter>);
    static_assert(are_forward_iter_type_requirements_ok<citer>);
}

BOOST_FIXTURE_TEST_CASE( iterator_multipass_equality, GicWithOneElementFixture ) {
    auto ita = container.begin();
    auto itb = container.begin();

    ASSERT_TRUE(ita == itb);
    BOOST_TEST(std::addressof(*ita) == std::addressof(*itb));
    ASSERT_TRUE(++ita == ++itb);
    ASSERT_TRUE(++ita == ++itb);
}

BOOST_FIXTURE_TEST_CASE( iterator_multipass_untied_copy,
                         GicWithOneElementFixture )
{
    auto ita = container.begin();
    auto itb = ita;
    ++itb;
    BOOST_TEST(*ita == NON_ZERO_VAL);
}


// ===== Bidirectional Iterator concept =====

BOOST_FIXTURE_TEST_CASE( iterator_pre_decrement, GicWithOneElementFixture ) {
    auto it = container.begin();
    ++it;

    std::add_lvalue_reference_t<decltype(it)> decremented = --it;
    BOOST_TEST(*decremented == NON_ZERO_VAL);
    ASSERT_TRUE(--(++decremented) == decremented);
    BOOST_TEST(&decremented == &it);
}

BOOST_FIXTURE_TEST_CASE( iterator_post_decrement, GicWithOneElementFixture ) {
    auto it = container.begin();
    ++it;

    static_assert(std::is_same_v<
        decltype(*it--),
        std::iterator_traits<decltype(it)>::reference>);

    auto it_copy = it;
    ASSERT_TRUE(it_copy == it--);
    BOOST_TEST(*it == NON_ZERO_VAL);
}


// GIC iterators cannot be random-access-iterators
