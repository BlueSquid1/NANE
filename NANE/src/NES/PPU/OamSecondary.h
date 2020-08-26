#ifndef OAM_SECONDARY
#define OAM_SECONDARY

#include <memory>
#include <vector>

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

    struct Indexed
    {
        int primaryOamIndex;
    };

    struct IndexedSprite : public Indexed
    {
        OamPrimary::Sprite sprite;
    };

    struct IndexPattern : IndexedSprite
    {
        patternIndex pattern;
    };

    struct IndexSpriteBuffer : public Indexed
    {
        OamPrimary::Sprite sprite;
        ScanlineTile scanlineTile;
    };

    struct SpritePatternTiles
    {
        int numOfTiles;
        PatternTables::BitTile firstTile;
        PatternTables::BitTile secondTile;
    };

    private:
    // Stores the number of sprites that have been fetched for the next scanline
    int spriteFetchNum;
    std::vector<int> fetchSpritesOamIndex = std::vector<int>(OamPrimary::TotalNumOfSprites);

    // Stores sprites for current scanline
    int activeSpriteBufferLen;
    std::vector<IndexSpriteBuffer> activeSpriteBuffer = std::vector<IndexSpriteBuffer>(OamPrimary::TotalNumOfSprites);
    

    public:
    OamSecondary();

    void AppendFetchedSprite(const OamPrimary::Sprite& sprite, int primaryOamIndex);

    OamSecondary::ScanlineTile CalcSpriteBuffer(int scanline, const OamPrimary::Sprite& sprite, const OamSecondary::SpritePatternTiles& spriteTile) const;
    
    OamSecondary::IndexPattern CalcForgroundPixel(int pixelX) const;

    OamSecondary::IndexedSprite GetFetchedSprite(int spriteNum) const;

    void ClearFetchData();

    void ClearActiveBuffer();

    // getters/setters
    int GetSpriteFetchNum() const;
    void AppendToActiveBuffer(const IndexSpriteBuffer& indexedScanline);
};

#endif