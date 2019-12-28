#include <catch2/catch.hpp>

#include "../../../src/NES/Util/BitUtil.h"

/**
 * tests if the nestest.nes rom can be parsed properly.
 */
TEST_CASE("Test array to vec") {
    const int arrayLen = 4;
    byte testArray[arrayLen] = {1, 5, 3, 5};
    std::unique_ptr<std::vector<byte>> testVecPointer  = BitUtil::ArrayToVec(testArray, arrayLen);
    std::vector testVec = *testVecPointer;

    for(int i = 0; i < arrayLen; ++i)
    {
        REQUIRE(testArray[i] == testVec[i]);
    }
}