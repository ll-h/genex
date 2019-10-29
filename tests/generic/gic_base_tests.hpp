#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include "generic_test_definitions.hpp"
#include "../zero_on_destruction.hpp"
using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE( emplace_get ) {
    gic_derived<int> container;

    auto key = container.emplace(NON_ZERO_VAL);
    gic_derived<int>::element_access_type maybe_val = container.get(key);

    BOOST_TEST(maybe_val != container.failed_get());
    BOOST_TEST(*maybe_val == NON_ZERO_VAL);
}

BOOST_AUTO_TEST_CASE( emplace_brackets_get ) {
    gic_derived<int> container;

    auto key = container.emplace(NON_ZERO_VAL);
    auto maybe_val = container[key];

    BOOST_TEST(maybe_val != container.failed_get());
    BOOST_TEST(*maybe_val == NON_ZERO_VAL);
}

BOOST_AUTO_TEST_CASE( get_const ) {
    gic_derived<int> container;
    gic_derived<int> const &container_const_ref = container;

    auto key = container.emplace(NON_ZERO_VAL);
    gic_derived<int>::element_const_access_type maybe_val =
            container_const_ref.get(key);

    BOOST_TEST(maybe_val != container_const_ref.failed_get());
    BOOST_TEST(*maybe_val == NON_ZERO_VAL);
}

BOOST_AUTO_TEST_CASE( remove_get ) {
    gic_derived<int> container;

    auto key = container.emplace(NON_ZERO_VAL);
    container.remove(key);
    auto maybe_val = container[key];

    BOOST_TEST(maybe_val == container.failed_get());
}

BOOST_AUTO_TEST_CASE( remove_old_key ) {
    gic_derived<int> container;

    auto old_key = container.emplace(NON_ZERO_VAL_1);
    container.remove(old_key);
    auto new_key = container.emplace(NON_ZERO_VAL_2);
    container.remove(old_key); // should have no effect
    auto maybe_val = container[new_key];

    BOOST_TEST(maybe_val != container.failed_get());
    BOOST_TEST(*maybe_val == NON_ZERO_VAL_2);
}

BOOST_AUTO_TEST_CASE( get_old_key ) {
    gic_derived<int> container;

    auto old_key = container.emplace(NON_ZERO_VAL_1);
    container.remove(old_key);
    auto new_key = container.emplace(NON_ZERO_VAL_2);
    (void)new_key;
    auto maybe_val = container[old_key];

    BOOST_TEST(maybe_val == container.failed_get());
}

BOOST_AUTO_TEST_CASE( destruction_on_raii ) {
    int val = NON_ZERO_VAL;

    {
        gic_derived<zero_on_destruction<int>> container;
        auto key = container.emplace(val);
        (void)key;
    }

    BOOST_TEST(val == 0);
}

BOOST_AUTO_TEST_CASE( destruction_on_removal ) {
    int val = NON_ZERO_VAL;

    gic_derived<zero_on_destruction<int>> container;
    auto key = container.emplace(val);
    container.remove(key);

    BOOST_TEST(val == 0);
}

BOOST_AUTO_TEST_CASE( destruction_on_emplacement ) {
    int val = NON_ZERO_VAL;

    gic_derived<zero_on_destruction<int>> container;
    auto key = container.emplace(val);
    container.remove(key);

    BOOST_TEST(val == 0);
}
