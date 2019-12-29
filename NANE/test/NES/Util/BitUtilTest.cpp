#include <catch2/catch.hpp>

#include "NES/Util/BitUtil.h"

/**
 * test valid array to vect
 */
TEST_CASE("Test valid array to vec") {
    const int arrayLen = 4;
    byte testArray[arrayLen] = {1, 5, 3, 5};
    std::unique_ptr<std::vector<byte>> testVecPointer  = BitUtil::ArrayToVec(testArray, arrayLen);
    std::vector testVec = *testVecPointer;

    for(int i = 0; i < arrayLen; ++i)
    {
        REQUIRE(testArray[i] == testVec[i]);
    }
}

/**
 * tests if empty array results in an empty vect
 */
TEST_CASE("Test empty array to vec") {
    byte emptyArray[0];
    std::unique_ptr<std::vector<byte>> testVecPointer  = BitUtil::ArrayToVec(emptyArray, 0);
    std::vector testVec = *testVecPointer;
    REQUIRE(testVec.size() == 0);
}

/**
 * tests if null array results in an null vect
 */
TEST_CASE("Test NULL array to vec") {
    std::unique_ptr<std::vector<byte>> testVecPointer  = BitUtil::ArrayToVec(NULL, 0);
    REQUIRE(testVecPointer.get() == NULL);
}


/**
 * tests if same vector results in vecequal returning true
 */
TEST_CASE("Test same vecs in vecEqual") {
    std::vector<byte> sameVec = {1, 2, 5, 24};
    REQUIRE(BitUtil::VecEqual(&sameVec, &sameVec) == true);
}

/**
 * test vecequal with different size vectors
 */
TEST_CASE("Test different size vecs in vecEqual") {
    std::vector<byte> vec1 = {1, 2, 5, 24};
    std::vector<byte> vec2 = {1, 2, 5};

    REQUIRE(BitUtil::VecEqual(&vec1, &vec2) == false);

    //should now be the same
    vec2.push_back(24);
    REQUIRE(BitUtil::VecEqual(&vec1, &vec2) == true);
}

/**
 * tests null vectors with vecEqual
 */
TEST_CASE("Test NULL vecs in vecEqual") {
    std::vector<byte> vec = {1, 2, 5, 24};
    REQUIRE(BitUtil::VecEqual(NULL, &vec) == false);
    REQUIRE(BitUtil::VecEqual(NULL, NULL) == true);
}

/**
 * tests a valid vector split to end
 */
TEST_CASE("Valid vector split to end") {
    std::vector<byte> vec = {1, 2, 5, 24, 5, 100};
    std::unique_ptr<std::vector<byte>> subVec = BitUtil::VecSplit(&vec, 2, vec.size());
    
    
    int len = vec.size() - 2;
    REQUIRE(subVec->size() == len);
    for(int i = 0; i < len; ++i)
    {
        REQUIRE( subVec->at(i) == vec[2 + i]);
    }
}

/**
 * tests a valid vector split at start
 */
TEST_CASE("Valid vector split at start") {
    std::vector<byte> vec = {1, 2, 5, 24, 5, 100};
    std::unique_ptr<std::vector<byte>> subVec = BitUtil::VecSplit(&vec, 0, 3);
    
    
    int len = 3;
    REQUIRE(subVec->size() == len);
    for(int i = 0; i < len; ++i)
    {
        REQUIRE( subVec->at(i) == vec[i]);
    }
}

/**
 * tests gitbits
 */
TEST_CASE("GetBits") {
    byte testByte = 93; //0101 1101
                        //|||| ||||
                        //7654 3210
    //start
    REQUIRE(BitUtil::GetBits(testByte, 0) == true);
    REQUIRE(BitUtil::GetBits(testByte, 0, 1) == 1);

    //middle
    REQUIRE(BitUtil::GetBits(testByte, 2) == true);
    REQUIRE(BitUtil::GetBits(testByte, 2, 5) == 7);

    //end
    REQUIRE(BitUtil::GetBits(testByte, 7) == false);
    REQUIRE(BitUtil::GetBits(testByte, 6, 7) == 1);
}