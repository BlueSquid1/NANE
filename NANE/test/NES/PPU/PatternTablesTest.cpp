#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/PPU/PatternTables.h"

TEST_CASE("make sure BitPatternTables is Plain-Old Data") {
    REQUIRE(std::is_pod<PatternTables::BitPatternTables>::value == true);
}

TEST_CASE("Test Get tile")
{
    std::vector<byte> pattern = 
    { 
        1, 0, 0, 0, 0, 0, 0, 2,
        0, 1, 0, 0, 0, 0, 2, 0,
        0, 0, 1, 0, 0, 2, 0, 0,
        0, 0, 0, 3, 3, 0, 0, 0,
        0, 0, 0, 3, 3, 0, 0, 0,
        0, 0, 2, 0, 0, 1, 0, 0,
        0, 2, 0, 0, 0, 0, 1, 0,
        2, 0, 0, 0, 0, 0, 0, 3
    };

    std::vector<byte> emptyPattern(64, 0);

    // lower plane for the pattern above
    std::vector<byte> lowerPlane = 
    {
        0x80,
        0x40,
        0x20,
        0x18,
        0x18,
        0x04,
        0x02,
        0x01
    };

    // upper plane for the pattern above
    std::vector<byte> upperPlane = 
    {
        0x01,
        0x02,
        0x04,
        0x18,
        0x18,
        0x20,
        0x40,
        0x81
    };

    std::shared_ptr<std::vector<byte>> chrRomVec = std::make_shared<std::vector<byte>>(0x2000);
    //set the first and last tile to the pattern
    for(int i = 0; i < 8; ++i)
    {
        chrRomVec->at(i) = lowerPlane.at(i);
        chrRomVec->at(i + 8) = upperPlane.at(i);
    }
    int offset = 0x2000 - 16;
    for(int i = 0; i < 8; ++i)
    {
        chrRomVec->at(offset + i) = lowerPlane.at(i);
        chrRomVec->at(offset + i + 8) = upperPlane.at(i);
    }

    PatternTables patternTables(chrRomVec);

    Matrix<byte> firstTile = patternTables.GetTile(0, 0, 0);
    REQUIRE(firstTile.dump() == pattern);
    Matrix<byte> secondTile = patternTables.GetTile(0, 0, 1);
    REQUIRE(secondTile.dump() == emptyPattern);

    Matrix<byte> lastTile = patternTables.GetTile(1, 15, 15);
    REQUIRE(lastTile.dump() == pattern);
    Matrix<byte> secondLastTile = patternTables.GetTile(1, 15, 14);
    REQUIRE(secondLastTile.dump() == emptyPattern);
}