#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include <utility>
#include <algorithm>
#include "generic_test_definitions.hpp"
using namespace boost::unit_test;

// ===== Iterator concept =====

BOOST_AUTO_TEST_CASE( deref_first_allocated ) {
    gic_derived<int> container;

    auto key = container.emplace(NON_ZERO_VAL);
    (void)key;
    auto it = container.begin();

    BOOST_TEST(*it == NON_ZERO_VAL);
}

BOOST_AUTO_TEST_CASE( deref_implicit_const ) {
    gic_derived<int> container;

    auto key = container.emplace(NON_ZERO_VAL);
    (void)key;
    auto it = std::as_const(container).begin();

    BOOST_TEST(*it == NON_ZERO_VAL);
}

BOOST_AUTO_TEST_CASE( deref_explicit_const ) {
    gic_derived<int> container;

    auto key = container.emplace(NON_ZERO_VAL);
    (void)key;
    auto it = container.cbegin();

    BOOST_TEST(*it == NON_ZERO_VAL);
}

BOOST_AUTO_TEST_CASE( deref_first_free ) {
    gic_derived<int> container;

    auto free_key = container.emplace(NON_ZERO_VAL_1);
    auto key = container.emplace(NON_ZERO_VAL_2);
    (void)key;
    container.remove(free_key);
    auto it = container.begin();

    BOOST_TEST(*it == NON_ZERO_VAL_2);
}

BOOST_AUTO_TEST_CASE( iter_swap ) {
    gic_derived<int> container;

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


// ===== Forward Iterator concept =====

/*
BOOST_AUTO_TEST_CASE( empty_container ) {
    gic_derived<int> container;
    BOOST_TEST(container.begin() == container.end());
    BOOST_TEST(container.cbegin() == container.cend());

    auto cc = std::as_const(container);
    BOOST_TEST(cc.begin() == cc.end());
}
//*/
