#ifndef NES_COLOUR
#define NES_COLOUR

#include "NES/Memory/Types.h"

class NesColour
{
    static const int colourIndex [64];

    union
    {
        qword raw;
        struct
        {
            byte blue;
            byte green;
            byte red;
            byte alpha;
        }channels;
    };
    byte nesColourIndex = 0;

    public:
    NesColour(byte nesColourIndex = 63);
};

#endif