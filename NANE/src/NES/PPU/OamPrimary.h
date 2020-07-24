#ifndef OAM
#define OAM

#include "NES/Memory/MemoryRepeaterArray.h"

#include "PatternTables.h"

typedef byte oamIndex;

class OamPrimary : public MemoryRepeaterArray
{
    public:
    struct Sprite
    {
        byte posY;
        patternIndex index;
        union
        {
            byte attributes;
            struct
            {
                byte palette : 2; //palette colour to use
                byte _ : 3; //unimplemented bits 
                bit backgroundPriority : 1; //0: in front of background; 1: behind background
                bit flipHorizontally : 1; //flip sprite horizontally
                bit flipVertically : 1; //flip sprite vertically
            };
        };
        byte posX;
    };

    static const int TotalNumOfSprites = 64;
    static const int rawLen = 256;

    struct OamStruct
    {
        // sprite at 0 has highest priority
        //sprite at 64 has lowest priority
        Sprite sprites[TotalNumOfSprites];
    };

    //anonymous union
    union
    {
        OamStruct name;
        byte raw[rawLen];
    };

    public:
    OamPrimary();

    //dissamble methods:
    std::string GenerateSpritesProperties() const;

    //getter/setters
    const OamPrimary::Sprite& GetSprite(int spriteNum) const;
};

#endif