#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include <detail/manually_destructed.hpp>
#include "finally.hpp"
#include "zero_on_destruction.hpp"
using namespace boost::unit_test;

using namespace genex::detail;

BOOST_AUTO_TEST_SUITE( manually_destroy )

BOOST_AUTO_TEST_CASE( construction ) {
    manually_destructed<int> wrap(55);
    BOOST_TEST(*wrap == 55);
}

BOOST_AUTO_TEST_CASE( automatic_destruction ) {
    int val = 55;
    {
        manually_destructed<zero_on_destruction<int>> wrap(val);
    }
    BOOST_TEST(val == 55);
}

BOOST_AUTO_TEST_CASE( erasure ) {
    int val = 55;
    manually_destructed<zero_on_destruction<int>> wrap(val);
    wrap.erase();
    BOOST_TEST(val == 0);
}

BOOST_AUTO_TEST_CASE( emplacement_after_erasure ) {
    manually_destructed<int> wrap(55);
    wrap.erase();
    wrap.emplace(0);
    BOOST_TEST(*wrap == 0);
}

BOOST_AUTO_TEST_CASE( emplacement_does_not_call_destructor ) {
    int val_a = 55;
    int val_b = 66;
    manually_destructed<zero_on_destruction<int>> wrap(val_a);
    wrap.emplace(val_b);
    BOOST_TEST(val_a == 55);
}

BOOST_AUTO_TEST_SUITE_END()

static bool empty_init() {
    return true;
}

int main(int argc, char* argv[], char* envp[]) {
    (void)envp;
    return ::boost::unit_test::unit_test_main( &empty_init, argc, argv );
}
