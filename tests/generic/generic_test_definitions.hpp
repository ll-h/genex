#ifndef GENERIC_TEST_DEFINITIONS_HPP
#define GENERIC_TEST_DEFINITIONS_HPP

// for IDEs
#ifndef OUTER_GIC_TEST
#include <split_gic.hpp>
using namespace genex;
template<typename... Args>
using gic_derived = split_gic<Args...>;
#endif

constexpr int
    NON_ZERO_VAL = 444,
    NON_ZERO_VAL_1 = NON_ZERO_VAL,
    NON_ZERO_VAL_2 = 5555;

#endif // GENERIC_TEST_DEFINITIONS_HPP
