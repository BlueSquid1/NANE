#ifndef NES_COLOUR
#define NES_COLOUR

#include "NES/Memory/Types.h"

union rawColour
{
    qword raw;
    struct
    {
        byte red;
        byte green;
        byte blue;
        byte _;
    }channels;
};

class NesColour
{
    private:
    static const rawColour colourIndex [64];

    public:
    static rawColour GetRawColour(byte nesColourIndex = 63);
};

#endif