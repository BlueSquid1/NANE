#ifndef OAM
#define OAM

#include "NES/Memory/MemoryRepeaterArray.h"

class Oam : public MemoryRepeaterArray
{
    struct Sprite
    {
        byte posY;
        byte index;
        union
        {
            byte attributes;
            struct
            {
                byte palette : 2; //palette colour to use
                bit _ : 3; //unimplemented bits 
                bit backgroundPriority : 1; //0: in front of background; 1: behind background
                bit flipHorizontally : 1; //flip sprite horizontally
                bit flipVertically : 1; //flip sprite vertically
            };
        };
        byte posX;
    };

    struct OamStruct
    {
        Sprite sprites[64];
    };

    static const int rawLen = 256;

    //anonymous union
    union
    {
        OamStruct name;
        byte raw[rawLen];
    };

    public:
    Oam();
};

#endif