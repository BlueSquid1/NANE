#include <catch2/catch.hpp>

#include "NES/Memory/Matrix.h"

/**
 * constructor tests
 */
TEST_CASE("constructor test") {
    int width = 5;
    int height = 3;
    int length = width * height;
    Matrix<int> blankMat(width,height, 0);
    const std::vector<int>& blankVec = blankMat.dump();
    REQUIRE(blankVec.size() == width * height);
    for(int i = 0; i < length; ++i)
    {
        REQUIRE(blankVec.at(i) == 0);
    }
    REQUIRE(blankMat.GetWidth() == width);
    REQUIRE(blankMat.GetHeight() == height);
}


/**
 * test get and set at an index
 */
TEST_CASE("get and set elements") {
    int width = 5;
    int height = 3;
    int length = width * height;
    Matrix<int> mat(width,height, 0);
    int startVal = 134;
    mat.Set(0,0, startVal);
    int midVal = 436;
    mat.Set(1, 4, midVal);
    int lastVal = 13;
    mat.Set(2, 4, lastVal);

    REQUIRE(mat.Get(0,0) == startVal);
    REQUIRE(mat.Get(0,1) == 0);

    REQUIRE(mat.Get(1,3) == 0);
    REQUIRE(mat.Get(1,4) == midVal);
    REQUIRE(mat.Get(2,0) == 0);

    REQUIRE(mat.Get(2,3) == 0);
    REQUIRE(mat.Get(2,4) == lastVal);

    const std::vector<int>& vecDump = mat.dump();
    REQUIRE(vecDump.size() == width * height);

    REQUIRE(vecDump.at(0) == startVal);
    REQUIRE(vecDump.at(1) == 0);

    REQUIRE(vecDump.at(8) == 0);
    REQUIRE(vecDump.at(9) == midVal);
    REQUIRE(vecDump.at(10) == 0);

    REQUIRE(vecDump.at(13) == 0);
    REQUIRE(vecDump.at(14) == lastVal);
}

/**
 * test region setting
 */
TEST_CASE("region setting") {
    int width = 3;
    int height = 6;
    Matrix<int> mat(width,height, 0);

    int startRegion = 34;
    mat.SetRegion(0, 0, 3, 2, startRegion);

    int endRegion = 54;
    mat.SetRegion(5, 1, 2, 1, endRegion);

    REQUIRE(mat.Get(0,0) == startRegion);
    REQUIRE(mat.Get(1, 2) == startRegion);
    REQUIRE(mat.Get(2, 0) == 0);

    REQUIRE(mat.Get(5, 0) == 0);
    REQUIRE(mat.Get(5, 1) == endRegion);
    REQUIRE(mat.Get(5, 2) == endRegion);

    const std::vector<int>& vecDump = mat.dump();
    REQUIRE(vecDump.at(0) == startRegion);
    REQUIRE(vecDump.at(5) == startRegion);
    REQUIRE(vecDump.at(6) == 0);

    REQUIRE(vecDump.at(15) == 0);
    REQUIRE(vecDump.at(16) == endRegion);
    REQUIRE(vecDump.at(17) == endRegion);
}