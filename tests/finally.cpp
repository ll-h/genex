#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include "finally.hpp"
using namespace boost::unit_test;


BOOST_AUTO_TEST_SUITE( finally_tests )


BOOST_AUTO_TEST_CASE( lambda ) {
    int val = 55;

    {
        finally unused_variable_name([&val]() {val = 0;});
        BOOST_TEST(val == 55);
    }

    BOOST_TEST(val == 0);
}


struct set_to_0_on_call {
    int& val;

    void operator()() {
        val = 0;
    }

    bool operator==(int other) {
        return val == other;
    }
};
BOOST_AUTO_TEST_CASE( callable ) {
    int val = 55;

    {
        finally _(set_to_0_on_call{val});
        BOOST_TEST(val == 55);
    }

    BOOST_TEST(val == 0);
}


BOOST_AUTO_TEST_CASE( does_nothing_before_destruction ) {
    int val = 55;
    finally _([&val]() {val = 0;});

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
