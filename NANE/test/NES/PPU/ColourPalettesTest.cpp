#include <catch2/catch.hpp>
#include <type_traits> //std::is_pod<>

#include "NES/PPU/ColourPalettes.h"

/**
 * test if PpuRegisters is a Plain-Old Data otherwise compiler might not place struct in the same order defined in the struct
 */
TEST_CASE("make sure ColourPalettes is Plain-Old Data otherwise") {
    REQUIRE(std::is_pod<ColourPalettes::paletteStruct>::value == true);
}

/**
 * test to make sure registers are cleared after constructor
 */
TEST_CASE("ColourPalettes cleared by default") {
    ColourPalettes palettes(0x3F00, 0x3F1F);

    REQUIRE(palettes.rawLen > 0);
    for(int i = 0; i < palettes.rawLen; ++i)
    {
        REQUIRE(palettes.raw[i] == 0);
    }
}

/**
 * test to make sure ColourPalettes can read and write properly
 */
TEST_CASE("ColourPalettes write to and read from raw") {
    ColourPalettes palettes(0x3F00, 0x3F1F);
    byte patternValue = 0;
    for(byte attributeIndex = 0; attributeIndex < 4; ++attributeIndex)
    {
        for(byte patternValue = 0; patternValue < 4; ++patternValue)
        {
            palettes.name.backgroundPalette[attributeIndex][patternValue] = (4 * attributeIndex) + patternValue;
        }
    }

    for(int spriteIndex = 0; spriteIndex < 4; ++spriteIndex)
    {
        palettes.name.spritePalette[spriteIndex].backgroundMirror = 16 + 2 * spriteIndex;
        palettes.name.spritePalette[spriteIndex].palette = 17 + 2 * spriteIndex;
    }

    for(int i = 0; i < palettes.rawLen; ++i)
    {
        REQUIRE(palettes.raw[i] == i);
    }
}

/**
 * test to make sure ColourPalettes can read and write properly
 */
TEST_CASE("ColourPalettes set read and write to mirror addresses") {
    ColourPalettes palettes(0x3F00, 0x3F1F);
    byte patternValue = 0;
    for(byte attributeIndex = 0; attributeIndex < 4; ++attributeIndex)
    {
        for(byte patternValue = 0; patternValue < 4; ++patternValue)
        {
            palettes.name.backgroundPalette[attributeIndex][patternValue] = (4 * attributeIndex) + patternValue;
        }
    }
    
    REQUIRE(palettes.Read(0x3F10) == palettes.Read(0x3F00));
    REQUIRE(palettes.Read(0x3F14) == palettes.Read(0x3F04));
    REQUIRE(palettes.Read(0x3F18) == palettes.Read(0x3F08));
    REQUIRE(palettes.Read(0x3F1C) == palettes.Read(0x3F0C));
}
