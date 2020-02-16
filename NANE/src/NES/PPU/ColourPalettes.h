#ifndef COLOUR_PALETTES
#define COLOUR_PALETTES

#include "NES/Memory/BitUtil.h"

class ColourPalettes
{
    struct paletteStruct
    {
        byte universalBackgroundColour;
        byte backgroundPalette0[3];
        byte _0;
        byte backgroundPalette1[3];
        byte _1;
        byte backgroundPalette2[3];
        byte _2;
        byte backgroundPalette3[3];
        byte _3;
        byte spritePalette0[3];
        byte _4;
        byte spritePalette1[3];
        byte _5;
        byte spritePalette2[3];
        byte _6;
        byte spritePalette3[3];
    };

    //anonymous union
    union
    {
        paletteStruct name;
        byte raw[sizeof(paletteStruct)];
    };
    const int rawLen = sizeof(paletteStruct);

    public:
    ColourPalettes();
};

#endif