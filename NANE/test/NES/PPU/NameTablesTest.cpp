#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/PPU/NameTables.h"
#include "NES/Cartridge/CartridgeMapping/CartridgeMapping0.h"

bool TestColour(int startX, int startY, QuadAreaPalette colours, const NameTables& nameTables)
{
    if(nameTables.GetPaletteIndex(startY, startX) != colours.topleftArea)
    {
        return false;
    }
    if(nameTables.GetPaletteIndex(startY + 1, startX + 1) != colours.topleftArea)
    {
        return false;
    }

    if(nameTables.GetPaletteIndex(startY, startX + 2) != colours.topRightArea)
    {
        return false;
    }
    if(nameTables.GetPaletteIndex(startY + 1, startX + 3) != colours.topRightArea)
    {
        return false;
    }

    if(nameTables.GetPaletteIndex(startY + 2, startX) != colours.bottomleftArea)
    {
        return false;
    }
    if(nameTables.GetPaletteIndex(startY + 3, startX + 1) != colours.bottomleftArea)
    {
        return false;
    }

    if(nameTables.GetPaletteIndex(startY + 2, startX + 2) != colours.bottomrightArea)
    {
        return false;
    }
    if(nameTables.GetPaletteIndex(startY + 3, startX + 3) != colours.bottomrightArea)
    {
        return false;
    }
    return true;
}


/**
 * test if NameTableStruct is a Plain-Old Data otherwise compiler might not place struct in the same order defined in the struct
 */
TEST_CASE("make sure NameTableStruct is Plain-Old Data otherwise") {
    REQUIRE(std::is_pod<NameTableStruct>::value == true);
    REQUIRE( sizeof(QuadAreaPalette) == 1 ); //really need this to be 1 byte in length
}

/**
 * test to make sure nameTables are cleared after constructor
 */
TEST_CASE("nameTables cleared by default") {
    NameTables nameTables;
    nameTables.SetMirrorType(INes::vertical);

    REQUIRE(nameTables.dataLen > 0);
    for(int i = 0; i < nameTables.dataLen; ++i)
    {
        REQUIRE(nameTables.Read(nameTables.startAddress + i) == 0);
    }
}

/**
 * test to make sure NameTable vertical mirroring works as expected
 */
TEST_CASE("NameTables Vertical mirroring") {
    NameTables verticalNameTables;
    verticalNameTables.SetMirrorType(INes::vertical);
    
    verticalNameTables.Write(0x2058, 129);
    REQUIRE(verticalNameTables.Read(0x2858) == 129);

    verticalNameTables.Write(0x2800, 35);
    REQUIRE(verticalNameTables.Read(0x2000) == 35);

    verticalNameTables.Write(0x2FFF, 43);
    REQUIRE(verticalNameTables.Read(0x27FF) == 43);
}

/**
 * test to make sure NameTable horizontal mirroring works as expected
 */
TEST_CASE("NameTables horizontal mirroring") {
    std::unique_ptr<ICartridge> nesRom = std::unique_ptr<CartridgeMapping0>( new CartridgeMapping0());
    NameTables horizontalNameTables;
    horizontalNameTables.SetMirrorType(INes::horizontal);
    
    horizontalNameTables.Write(0x2058, 129);
    REQUIRE(horizontalNameTables.Read(0x2458) == 129);

    horizontalNameTables.Write(0x2400, 35);
    REQUIRE(horizontalNameTables.Read(0x2000) == 35);

    horizontalNameTables.Write(0x27FF, 43);
    REQUIRE(horizontalNameTables.Read(0x23FF) == 43);

    horizontalNameTables.Write(0x2C00, 98);
    REQUIRE(horizontalNameTables.Read(0x2800) == 98);

    horizontalNameTables.Write(0x2FFF, 111);
    REQUIRE(horizontalNameTables.Read(0x2BFF) == 111);
}

TEST_CASE("Test GetPatternIndex in all 4 corners of first nametable")
{
    NameTables nameTables;
    nameTables.SetMirrorType(INes::vertical);

    patternIndex patternTopLeft = 12;
    patternIndex patternTopRight = 200;
    patternIndex patternBottomLeft = 255;
    patternIndex patternBottomRight = 0;

    
    nameTables.Write(0x2000, patternTopLeft);
    nameTables.Write(0x201F, patternTopRight);
    nameTables.Write(0x23a0, patternBottomLeft);
    nameTables.Write(0x23bF, patternBottomRight);
    REQUIRE( nameTables.Read(0x2000) == patternTopLeft );
    REQUIRE( nameTables.Read(0x201F) == patternTopRight );
    REQUIRE( nameTables.Read(0x23a0) == patternBottomLeft );
    REQUIRE( nameTables.Read(0x23bF) == patternBottomRight );

    REQUIRE(nameTables.GetPatternIndex(0, 0) == patternTopLeft);
    REQUIRE(nameTables.GetPatternIndex(0, 31) == patternTopRight);
    REQUIRE(nameTables.GetPatternIndex(29, 0) == patternBottomLeft);
    REQUIRE(nameTables.GetPatternIndex(29, 31) == patternBottomRight);
}

TEST_CASE("Test getting palette colours for all 4 corners in first nametable")
{
    NameTables nameTables;
    nameTables.SetMirrorType(INes::vertical);

    QuadAreaPalette paletteTopLeft;
    paletteTopLeft.topleftArea = 0;
    paletteTopLeft.topRightArea = 1;
    paletteTopLeft.bottomleftArea = 2;
    paletteTopLeft.bottomrightArea = 3;
    REQUIRE(paletteTopLeft.value == 0xE4);

    QuadAreaPalette paletteTopRight;
    paletteTopRight.topleftArea = 3;
    paletteTopRight.topRightArea = 2;
    paletteTopRight.bottomleftArea = 1;
    paletteTopRight.bottomrightArea = 0;
    REQUIRE(paletteTopRight.value == 0x1B);

    QuadAreaPalette paletteBottomLeft;
    paletteBottomLeft.topleftArea = 3;
    paletteBottomLeft.topRightArea = 1;
    paletteBottomLeft.bottomleftArea = 2;
    paletteBottomLeft.bottomrightArea = 0;
    REQUIRE(paletteBottomLeft.value == 0x27);

    QuadAreaPalette paletteBottomRight;
    paletteBottomRight.topleftArea = 1;
    paletteBottomRight.topRightArea = 0;
    paletteBottomRight.bottomleftArea = 2;
    paletteBottomRight.bottomrightArea = 3;
    REQUIRE(paletteBottomRight.value == 0xE1);

    
    nameTables.Write(0x23C0, paletteTopLeft.value);
    nameTables.Write(0x23C7, paletteTopRight.value);
    nameTables.Write(0x23F0, paletteBottomLeft.value);
    nameTables.Write(0x23F7, paletteBottomRight.value);
    REQUIRE( nameTables.Read(0x23C0) == paletteTopLeft.value );
    REQUIRE( nameTables.Read(0x23C7) == paletteTopRight.value );
    REQUIRE( nameTables.Read(0x23F0) == paletteBottomLeft.value );
    REQUIRE( nameTables.Read(0x23F7) == paletteBottomRight.value );

    REQUIRE( TestColour(0, 0, paletteTopLeft, nameTables) == true );
    REQUIRE( TestColour(28, 0, paletteTopRight, nameTables) == true );
    REQUIRE( TestColour(0, 24, paletteBottomLeft, nameTables) == true );
    REQUIRE( TestColour(28, 24, paletteBottomRight, nameTables) == true );
}

TEST_CASE("Test CalcBgrFetchTile()")
{
    NameTables nameTables;

    Point pixelTopLeft;
    pixelTopLeft.x = 0;
    pixelTopLeft.y = 0;

    Point pixelTopRight;
    pixelTopRight.x = 511;
    pixelTopRight.y = 0;

    Point pixelBottomLeft;
    pixelBottomLeft.x = 0;
    pixelBottomLeft.y = 479;

    Point pixelBottomRight;
    pixelBottomRight.x = 511;
    pixelBottomRight.y = 479;

    Point tileTopLeft = nameTables.CalcBgrFetchTile(pixelTopLeft);
    REQUIRE( tileTopLeft.x == 0 );
    REQUIRE( tileTopLeft.y == 0 );

    Point tileTopRight = nameTables.CalcBgrFetchTile(pixelTopRight);
    REQUIRE( tileTopRight.x == 63 );
    REQUIRE( tileTopRight.y == 0 );

    Point tileBottomLeft = nameTables.CalcBgrFetchTile(pixelBottomLeft);
    REQUIRE( tileBottomLeft.x == 0 );
    REQUIRE( tileBottomLeft.y == 59 );

    Point tileBottomRight = nameTables.CalcBgrFetchTile(pixelBottomRight);
    REQUIRE( tileBottomRight.x == 63 );
    REQUIRE( tileBottomRight.y == 59 );
}