#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include "detail/fit_slot.hpp"
using namespace boost::unit_test;

constexpr int
    NON_ZERO_VAL = 444,
    NON_ZERO_VAL_1 = NON_ZERO_VAL,
    NON_ZERO_VAL_2 = 5555;


BOOST_AUTO_TEST_SUITE( fit_slot_tests )

struct object_type {
    int& val;

    object_type(int& v) : val(v) {}
    ~object_type() {
        val = 0;
    }
};
using slot_type = genex::detail::fit_slot<object_type, int>;
using instrument_slot = genex::detail::fit_slot<object_type, object_type>;

BOOST_AUTO_TEST_CASE( default_construction ) {
    slot_type slot;
    BOOST_TEST(slot.index() == 0);
}

BOOST_AUTO_TEST_CASE( construction_with_value ) {
    int val = NON_ZERO_VAL;
    slot_type slot(std::in_place_index<1>, val);

    BOOST_TEST(slot.index() == 1);
    BOOST_TEST((genex::get<1>(slot).val == NON_ZERO_VAL));
}

BOOST_AUTO_TEST_CASE( emplace_value ) {
    int val_1 = NON_ZERO_VAL_1;
    int val_2 = NON_ZERO_VAL_2;
    instrument_slot slot(std::in_place_index<1>, val_1);
    slot.emplace<0>(val_2);
    val_1 = NON_ZERO_VAL_1;

    slot.emplace<1>(val_1);
    BOOST_TEST(slot.index() == 1);
    BOOST_TEST(val_2 == 0);
    BOOST_TEST((genex::get<1>(slot).val == NON_ZERO_VAL_1));
}

BOOST_AUTO_TEST_CASE( emplace_free_index ) {
    int val_1 = NON_ZERO_VAL_1;
    int val_2 = NON_ZERO_VAL_2;
    instrument_slot slot(std::in_place_index<1>, val_1);
    slot.emplace<0>(val_2);

    BOOST_TEST(slot.index() == 0);
    BOOST_TEST(val_1 == 0);
    BOOST_TEST((genex::get<0>(slot).val == NON_ZERO_VAL_2));
}


BOOST_AUTO_TEST_CASE( copy_empty_in_empty ) {
    int val_1;
    int val_2;
    instrument_slot slot_a(std::in_place_index<1>, val_1);
    instrument_slot slot_b(std::in_place_index<1>, val_2);
    slot_a.emplace<0>(val_1);
    slot_b.emplace<0>(val_2);
    val_1 = NON_ZERO_VAL_1;
    val_2 = NON_ZERO_VAL_2;

    slot_a = slot_b;

    // unaffected index
    BOOST_TEST(slot_a.index() == 0);
    // correct value in slot a
    BOOST_TEST((genex::get<0>(slot_a).val == NON_ZERO_VAL_2));
    // val_1 was not set to 0
    BOOST_TEST(val_1 == NON_ZERO_VAL_1);
}

BOOST_AUTO_TEST_CASE( copy_occupied_in_occupied ) {
    int val_1;
    int val_2;
    instrument_slot slot_a(std::in_place_index<1>, val_1);
    instrument_slot slot_b(std::in_place_index<1>, val_2);
    val_1 = NON_ZERO_VAL_1;
    val_2 = NON_ZERO_VAL_2;

    slot_a = slot_b;

    // unaffected index
    BOOST_TEST(slot_a.index() == 1);
    // correct value in slot a
    BOOST_TEST((genex::get<0>(slot_a).val == NON_ZERO_VAL_2));
    // val_1 was not set to 0
    BOOST_TEST(val_1 == NON_ZERO_VAL_1);
}

BOOST_AUTO_TEST_SUITE_END()

static bool empty_init() {
    return true;
}

int main(int argc, char* argv[], char* envp[]) {
    (void)envp;
    return ::boost::unit_test::unit_test_main( &empty_init, argc, argv );
}
