#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/PPU/OamPrimary.h"

/**
 * test if PpuRegisters is a Plain-Old Data otherwise compiler might not place struct in the same order defined in the struct
 */
TEST_CASE("make sure OamStruct is Plain-Old Data otherwise") {
    REQUIRE(std::is_pod<OamPrimary::OamStruct>::value == true);
}

/**
 * Test to make sure OAM defautlts to 0xFF.
 */
TEST_CASE("Oam defaults to 0xFF") {
    OamPrimary oam;

    REQUIRE(oam.dataLen > 0);
    for(int i = 0; i < oam.dataLen; ++i)
    {
        REQUIRE(oam.Read(oam.startAddress + i) == 0xFF);
    }
}

/**
 * Test GetSprite method.
 */
TEST_CASE("Test get sprite") 
{
    OamPrimary oam;

    byte firstSpriteY = 24;
    byte firstSpriteIndex = 100;
    byte firstSpriteAttributes = 189;
    byte firstSpriteX = 0;
    oam.Write(0x0, firstSpriteY);
    oam.Write(0x1, firstSpriteIndex);
    oam.Write(0x2, firstSpriteAttributes);
    oam.Write(0x3, firstSpriteX);

    byte lastSpriteY = 81;
    byte lastSpriteIndex = 112;
    byte lastSpriteAttributes = 69;
    byte lastSpriteX = 173;
    oam.Write(0xFC, lastSpriteY);
    oam.Write(0xFD, lastSpriteIndex);
    oam.Write(0xFE, lastSpriteAttributes);
    oam.Write(0xFF, lastSpriteX);

    const OamPrimary::Sprite& fetchedFirstSprite = oam.GetSprite(0);
    const OamPrimary::Sprite& fetchedLastSprite = oam.GetSprite(63);

    REQUIRE(fetchedFirstSprite.posY == firstSpriteY);
    REQUIRE(fetchedFirstSprite.index == firstSpriteIndex);
    REQUIRE(fetchedFirstSprite.attributes == firstSpriteAttributes);
    REQUIRE(fetchedFirstSprite.posX == firstSpriteX);

    REQUIRE(fetchedLastSprite.posY == lastSpriteY);
    REQUIRE(fetchedLastSprite.index == lastSpriteIndex);
    REQUIRE(fetchedLastSprite.attributes == lastSpriteAttributes);
    REQUIRE(fetchedLastSprite.posX == lastSpriteX);
}