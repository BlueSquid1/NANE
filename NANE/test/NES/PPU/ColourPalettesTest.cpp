#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/PPU/ColourPalettes.h"
#include "NES/PPU/NesColour.h"

static const uint8_t testRomPalettes[16] = 
{
    0x0f, 0x06, 0x12, 0x33, 0x33, 0x06, 0x12, 0x33, 0x38, 0x06, 0x12, 0x33, 0x3a, 0x06, 0x12, 0x33
};

/**
 * test if PpuRegisters is a Plain-Old Data otherwise compiler might not place struct in the same order defined in the struct
 */
TEST_CASE("make sure ColourPalettes is Plain-Old Data otherwise") {
    REQUIRE(std::is_pod<paletteStruct>::value == true);
}

/**
 * test to make sure registers are cleared after constructor
 */
TEST_CASE("ColourPalettes cleared by default") {
    ColourPalettes palettes;

    REQUIRE(palettes.dataLen > 0);
    for(int i = 0; i < palettes.dataLen; ++i)
    {
        REQUIRE(palettes.Read(palettes.startAddress + i) == 0);
    }
}

/**
 * test to make sure ColourPalettes can read and write properly
 */
TEST_CASE("ColourPalettes write to and read from raw") {
    ColourPalettes palettes;

    for(int i = 0; i < 16; ++i)
    {
        palettes.Write( 0x3F00 + i, testRomPalettes[i]);
    }

    //write to background palette 0
    REQUIRE(palettes.Read(0x3F00) == 0x0F);
    REQUIRE(palettes.Read(0x3F01) == 0x06);
    REQUIRE(palettes.Read(0x3F03) == 0x33);

    //test redirrecting from palette 0
    REQUIRE(palettes.Read(0x3F00) == 0x0F);
    REQUIRE(palettes.Read(0x3F10) == 0x0F);

    //write to background palette 3
    palettes.Write(0x3F0C, 0x3a);
    palettes.Write(0x3F0D, 0x06);
    palettes.Write(0x3F0F, 0x33);
    REQUIRE(palettes.Read(0x3F0C) == 0x3a);
    REQUIRE(palettes.Read(0x3F0D) == 0x06);
    REQUIRE(palettes.Read(0x3F0F) == 0x33);

    //test redirrecting from palette 3
    REQUIRE(palettes.Read(0x3F0C) == 0x3a);
    REQUIRE(palettes.Read(0x3F1C) == 0x3a);
}

/**
 * test ColourPalettes calls PatternValueToColour
 */
TEST_CASE("ColourPalettes calls PatternValueToColour") {
    ColourPalettes palettes;

    for(int i = 0; i < 16; ++i)
    {
        palettes.Write( 0x3F00 + i, testRomPalettes[i]);
    }

    for(int palette = 0; palette < 4; ++palette)
    {
        for(int colourNum = 0; colourNum < 4; ++colourNum)
        {
            int i = palette * 4 + colourNum;
            REQUIRE( palettes.PatternValueToColour(palette, colourNum).raw == NesColour::GetRawColour(testRomPalettes[i]).raw );
        }
    }

    //write to background palette 0
    REQUIRE(palettes.Read(0x3F00) == 0x0F);
}