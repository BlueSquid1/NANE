#include <catch2/catch.hpp>

#include "NES/Memory/MemoryRepeaterArray.h"
#include "NES/Memory/MemoryRepeaterVec.h"

TEST_CASE("LowerOffset checks") {
    std::shared_ptr<std::vector<byte>> data(new std::vector<byte>(50));
    MemoryRepeaterVec repeater(0, 99, data);
    REQUIRE(repeater.LowerOffset(0) == 0 );
    REQUIRE(repeater.LowerOffset(49) == 49 );
    REQUIRE(repeater.LowerOffset(50) == 0 );
    REQUIRE(repeater.LowerOffset(99) == 49 );
}

TEST_CASE("LowerOffset checks without vector") {
    MemoryRepeaterArray repeater(0, 99, NULL, 50);
    REQUIRE(repeater.LowerOffset(0) == 0 );
    REQUIRE(repeater.LowerOffset(49) == 49 );
    REQUIRE(repeater.LowerOffset(50) == 0 );
    REQUIRE(repeater.LowerOffset(99) == 49 );
}

TEST_CASE("Read and Write tests") {
    std::shared_ptr<std::vector<byte>> data(new std::vector<byte>(50));
    MemoryRepeaterVec repeater(0, 99, data);
    repeater.Write(0, 24);
    REQUIRE(repeater.Read(0) == 24);
    REQUIRE(repeater.Read(50) == 24);

    repeater.Write(49, 111);
    REQUIRE(repeater.Read(49) == 111);
    REQUIRE(repeater.Read(99) == 111);
}

TEST_CASE("contains tests") {
    MemoryRepeaterArray repeater(0, 99, NULL, 10);

    REQUIRE(repeater.Contains(0) == true);
    REQUIRE(repeater.Contains(1) == true);
    REQUIRE(repeater.Contains(9) == true);
    REQUIRE(repeater.Contains(10) == true);
    REQUIRE(repeater.Contains(99) == true);
    REQUIRE(repeater.Contains(100) == false);
}