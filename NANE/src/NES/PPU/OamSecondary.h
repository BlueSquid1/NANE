#ifndef OAM_SECONDARY
#define OAM_SECONDARY

#include <memory>

#include "OamPrimary.h"
#include "NesColour.h"
#include "NES/Memory/BitUtil.h"


class OamSecondary : public OamPrimary
{
    public:
    struct ScanlineTile
    {
        byte lsbSpriteTile;
        byte msbSpriteTile;
    };

    struct SpriteProperties
    {
        ScanlineTile scanlineTile;
        int primaryOamIndex;
    };

    struct SpritePixel
    {
        patternIndex pattern;
        int primaryOamIndex;
    };

    private:
    int activeSpriteNum;

    SpriteProperties SpriteBuffers[OamPrimary::TotalNumOfSprites];
    

    public:
    OamSecondary();

    void AppendSprite(const OamPrimary::Sprite& sprite, int primaryOamIndex);

    std::unique_ptr<OamSecondary::ScanlineTile> CalcSpriteBuffer(int scanline, const OamPrimary::Sprite& sprite, const PatternTables::BitTile& spriteTile) const;
    
    OamSecondary::SpritePixel CalcForgroundPixel(int curCycle) const;

    void Clear();

    // getters/setters
    int GetActiveSpriteNum() const;
    const OamSecondary::ScanlineTile& GetSpriteScanlineTile(int spriteNum) const;
    void SetSpriteScanlineTile(int spriteNum, const OamSecondary::ScanlineTile& spriteBuffer);
};

#endif