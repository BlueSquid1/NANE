#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/PPU/ColourPalettes.h"

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

    //write to background palette 0
    palettes.Write(0x3F00, 43);
    palettes.Write(0x3F01, 83);
    palettes.Write(0x3F03, 123);
    REQUIRE(palettes.Read(0x3F00) == 43);
    REQUIRE(palettes.Read(0x3F01) == 83);
    REQUIRE(palettes.Read(0x3F03) == 123);

    //test redirrecting from palette 0
    REQUIRE(palettes.Read(0x3F00) == 43);
    REQUIRE(palettes.Read(0x3F10) == 43);

    //write to background palette 3
    palettes.Write(0x3F0C, 74);
    palettes.Write(0x3F0D, 6);
    palettes.Write(0x3F0F, 34);
    REQUIRE(palettes.Read(0x3F0C) == 74);
    REQUIRE(palettes.Read(0x3F0D) == 6);
    REQUIRE(palettes.Read(0x3F0F) == 34);

    //test redirrecting from palette 3
    REQUIRE(palettes.Read(0x3F0C) == 74);
    REQUIRE(palettes.Read(0x3F1C) == 74);
}
