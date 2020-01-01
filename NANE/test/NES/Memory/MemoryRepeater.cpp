#include <catch2/catch.hpp>

#include "NES/Memory/MemoryRepeater.h"

TEST_CASE("LowerOffset checks") {
    std::unique_ptr<std::vector<byte>> data(new std::vector<byte>(50));
    MemoryRepeater repeater(0, 99, std::move(data));
    REQUIRE(repeater.LowerOffset(0) == 0 );
    REQUIRE(repeater.LowerOffset(49) == 49 );
    REQUIRE(repeater.LowerOffset(50) == 0 );
    REQUIRE(repeater.LowerOffset(99) == 49 );
}

TEST_CASE("LowerOffset checks without vector") {
    MemoryRepeater repeater(0, 99, 50);
    REQUIRE(repeater.LowerOffset(0) == 0 );
    REQUIRE(repeater.LowerOffset(49) == 49 );
    REQUIRE(repeater.LowerOffset(50) == 0 );
    REQUIRE(repeater.LowerOffset(99) == 49 );
}

TEST_CASE("Read and Write tests") {
    std::unique_ptr<std::vector<byte>> data(new std::vector<byte>(50));
    MemoryRepeater repeater(0, 99, std::move(data));
    repeater.Write(0, 24);
    REQUIRE(repeater.Read(0) == 24);
    REQUIRE(repeater.Read(50) == 24);

    repeater.Write(49, 111);
    REQUIRE(repeater.Read(49) == 111);
    REQUIRE(repeater.Read(99) == 111);
}

TEST_CASE("contains tests") {
    std::unique_ptr<std::vector<byte>> data(new std::vector<byte>(50));
    MemoryRepeater repeater(0, 99, 10);

    REQUIRE(repeater.Contains(0) == true);
    REQUIRE(repeater.Contains(1) == true);
    REQUIRE(repeater.Contains(9) == true);
    REQUIRE(repeater.Contains(10) == true);
    REQUIRE(repeater.Contains(99) == true);
    REQUIRE(repeater.Contains(100) == false);
}