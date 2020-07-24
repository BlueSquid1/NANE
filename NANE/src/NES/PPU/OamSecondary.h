#ifndef OAM_SECONDARY
#define OAM_SECONDARY

#include "OamPrimary.h"
#include "NesColour.h"
#include "NES/Memory/Types.h"


class OamSecondary : public OamPrimary
{
    private:
    int activeSpriteNum;

    byte lsbSpriteTiles[OamPrimary::TotalNumOfSprites];
    byte msbSpriteTiles[OamPrimary::TotalNumOfSprites];

    private:
    

    public:
    OamSecondary();

    void AppendSprite(const OamPrimary::Sprite& sprite);

    void PopulateOamTableBuffers(int scanline, int curCycle);
    
    rawColour CalcForegroundPixel(int curCycle) const;

    void Clear();

    // getters/setters
    int GetActiveSpriteNum() const;
    const byte& GetLsbSpriteTiles(int spriteNum) const;
    const byte& GetMsbSpriteTiles(int spriteNum) const;
};

#endif