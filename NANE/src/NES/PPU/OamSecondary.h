#ifndef OAM_SECONDARY
#define OAM_SECONDARY

#include "Oam.h"
#include "NesColour.h"
#include "NES/Memory/Types.h"


class OamSecondary : public Oam
{
    private:
    int activeSpriteNum;

    byte lsbSpriteTiles[Oam::TotalNumOfSprites];
    byte msbSpriteTiles[Oam::TotalNumOfSprites];

    private:
    

    public:
    OamSecondary();

    void AppendSprite(const Oam::Sprite& sprite);

    void PopulateOamTableBuffers(int scanline, int curCycle);
    
    rawColour CalcForegroundPixel(int curCycle) const;

    void Clear();

    // getters/setters
    int GetActiveSpriteNum() const;
    const byte& GetLsbSpriteTiles(int spriteNum) const;
    const byte& GetMsbSpriteTiles(int spriteNum) const;
};

#endif