#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include <split_gic.hpp>
using namespace boost::unit_test;

using namespace genex;

BOOST_AUTO_TEST_SUITE( split_gic_tests )

using gic_derived = split_gic<int>;
#define OUTER_GIC_TEST
#include "generic/gic_base.hpp"

BOOST_AUTO_TEST_SUITE_END()

static bool empty_init() {
    return true;
}

int main(int argc, char* argv[], char* envp[]) {
    (void)envp;
    return ::boost::unit_test::unit_test_main( &empty_init, argc, argv );
}
