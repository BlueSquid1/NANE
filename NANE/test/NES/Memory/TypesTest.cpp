#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/Memory/Types.h"

/**
 * @brief PODs test.
 */
TEST_CASE("make sure dword_p and qword_p are Plain-Old Data otherwise") {
    REQUIRE(std::is_pod<dword_p>::value == true);
    REQUIRE(std::is_pod<qword_p>::value == true);
}

/**
 * @brief Tests dword_p and qword_p bit shifting.
 */
TEST_CASE("dword_p and qword_p setting") {
    dword_p dValue;
    qword_p qValue;
    dValue.val = 0x3456;
    qValue.val = 0x12345678;
    REQUIRE(dValue.upper == 0x34);
    REQUIRE(dValue.lower == 0x56);
    REQUIRE(qValue.first.lower == 0x78);
    REQUIRE(qValue.first.upper == 0x56);
    REQUIRE(qValue.second.lower == 0x34);
    REQUIRE(qValue.second.upper == 0x12);
}

/**
 * @brief Tests dword_p and qword_p bit shifting.
 */
TEST_CASE("dword_p and qword_p bit shifting") {
    dword_p dValue;
    qword_p qValue;
    dValue.upper = 0x1;
    dValue.val >>= 1;
    qValue.second.val = 0x1;
    qValue.val >>= 1;
    REQUIRE(dValue.lower == 0x80);
    REQUIRE(qValue.first.upper == 0x80);
}