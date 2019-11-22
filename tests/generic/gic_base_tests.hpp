#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
#include <memory>
#include <boost/test/unit_test.hpp>
#include "generic_test_definitions.hpp"
#include "../zero_on_destruction.hpp"
using namespace boost::unit_test;


// ===== Emplacing =====

BOOST_FIXTURE_TEST_CASE( emplace_get, GicWithOneElementFixture ) {
    gic_type::element_access_type maybe_val = container.get(key);

    // Boost.Test tries to print maybe_val without the extra pair of parenthesis
    BOOST_TEST((maybe_val != container.failed_get()));
    BOOST_TEST(*maybe_val == NON_ZERO_VAL);
}

BOOST_FIXTURE_TEST_CASE( emplace_brackets_get, GicWithOneElementFixture ) {
    auto maybe_val = container[key];

    BOOST_TEST((maybe_val != container.failed_get()));
    BOOST_TEST(*maybe_val == NON_ZERO_VAL);
}

BOOST_FIXTURE_TEST_CASE( emplace_and_get, GicFixture ) {
    auto [key, elem] = container.emplace_and_get(NON_ZERO_VAL);
    auto maybe_val = container[key];

    BOOST_TEST((maybe_val != container.failed_get()));
    BOOST_TEST(elem == NON_ZERO_VAL);
    BOOST_TEST(std::addressof(elem) == std::addressof(*maybe_val));
}

struct has_its_own_key {
    using key_type = genex::key<has_its_own_key>;

    key_type const key;
    int val;

    has_its_own_key(key_type const& k, int v) : key(k), val(v) {}
    has_its_own_key(int v, key_type&& k) : key(k), val(v) {}
};

BOOST_AUTO_TEST_CASE( emplace_with_key_by_cref ) {
    gic_derived<has_its_own_key> container;
    auto [key, elem] = container.emplace_and_get(genex::key_placeholder,
                                                 NON_ZERO_VAL);

    BOOST_TEST(elem.val == NON_ZERO_VAL);
    BOOST_TEST((elem.key == key));
}

BOOST_AUTO_TEST_CASE( emplace_with_key_by_rv_ref ) {
    gic_derived<has_its_own_key> container;
    auto [key, elem] = container.emplace_and_get(NON_ZERO_VAL,
                                                 genex::key_placeholder);

    BOOST_TEST(elem.val == NON_ZERO_VAL);
    BOOST_TEST((elem.key == key));
}


BOOST_FIXTURE_TEST_CASE( get_const, GicWithOneElementFixture ) {
    gic_type const& container_const_ref = container;

    gic_type::element_const_access_type maybe_val =
            container_const_ref.get(key);

    BOOST_TEST((maybe_val != container_const_ref.failed_get()));
    BOOST_TEST(*maybe_val == NON_ZERO_VAL);
}

BOOST_FIXTURE_TEST_CASE( remove_get, GicWithOneElementFixture ) {
    container.remove(key);
    auto maybe_val = container[key];

    BOOST_TEST((maybe_val == container.failed_get()));
}

BOOST_FIXTURE_TEST_CASE( remove_old_key, GicFixture ) {
    auto old_key = container.emplace(NON_ZERO_VAL_1);
    container.remove(old_key);
    auto new_key = container.emplace(NON_ZERO_VAL_2);
    container.remove(old_key); // should have no effect
    auto maybe_val = container[new_key];

    BOOST_TEST((maybe_val != container.failed_get()));
    BOOST_TEST(*maybe_val == NON_ZERO_VAL_2);
}

BOOST_FIXTURE_TEST_CASE( get_old_key, GicFixture ) {
    auto old_key = container.emplace(NON_ZERO_VAL_1);
    container.remove(old_key);
    auto new_key = container.emplace(NON_ZERO_VAL_2);
    (void)new_key;
    auto maybe_val = container[old_key];

    BOOST_TEST((maybe_val == container.failed_get()));
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
