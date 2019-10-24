#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

static const int
    NON_ZERO_VAL = 444,
    NON_ZERO_VAL_1 = NON_ZERO_VAL,
    NON_ZERO_VAL_2 = 5555;

BOOST_AUTO_TEST_CASE( emplace_get ) {
    gic_derived container;

    auto key = container.emplace(NON_ZERO_VAL);
    auto maybe_val = container.get(key);

    BOOST_TEST(maybe_val != container.failed_get());
    BOOST_TEST(*maybe_val == NON_ZERO_VAL);
}
