#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include "zero_on_destruction.hpp"
using namespace boost::unit_test;


BOOST_AUTO_TEST_SUITE( zero_on_destruction_tests )

BOOST_AUTO_TEST_CASE( after_destruction ) {
    int val = 55;

    {
        zero_on_destruction z(val);
    }

    BOOST_TEST(val == 0);
}


BOOST_AUTO_TEST_CASE( does_nothing_before_destruction ) {
    int val = 55;
    zero_on_destruction z(val);
    BOOST_TEST(val == 55);
}



BOOST_AUTO_TEST_SUITE_END()

static bool empty_init() {
    return true;
}

int main(int argc, char* argv[], char* envp[]) {
    (void)envp;
    return ::boost::unit_test::unit_test_main( &empty_init, argc, argv );
}
