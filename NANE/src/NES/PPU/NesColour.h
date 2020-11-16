#ifndef NES_COLOUR
#define NES_COLOUR

#include "NES/Memory/Types.h"

typedef byte colourIndex;

union rawColour
{
    qword raw;
    #pragma pack(push, 1)
    struct
    {
        byte alpha;
        byte blue;
        byte green;
        byte red;
    }channels;
    #pragma pack(pop)
};

class NesColour
{
    private:
    static const rawColour colourLookupIndex [64];

    public:
    static rawColour GetRawColour(colourIndex nesColourIndex = 63);
};

#endif