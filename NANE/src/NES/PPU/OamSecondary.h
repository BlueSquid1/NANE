#ifndef OAM_SECONDARY
#define OAM_SECONDARY

#include <memory>
#include <vector>

#include "OamPrimary.h"
#include "NesColour.h"
#include "NES/Memory/BitUtil.h"


class OamSecondary : public OamPrimary
{
    private:
    /**
     * @brief Structure to track the primary OAM index.
     */
    struct Indexed
    {
        int primaryOamIndex;
    };

    public:
    /**
     * @brief Holds the data from a row in a scanline.
     */
    struct ScanlineTile
    {
        byte lsbSpriteTile;
        byte msbSpriteTile;
    };

    /**
     * @brief Sprite and it's corresponding primary OAM index.
     */
    struct IndexedSprite : public Indexed
    {
        OamPrimary::Sprite sprite;
    };

    /**
     * @brief Pattern index and it's corresponding primary OAM index.
     */
    struct IndexPattern : IndexedSprite
    {
        patternIndex pattern;
    };

    /**
     * @brief Sprite, scanline row and it's corresponding primary OAM index.
     */
    struct IndexSpriteBuffer : public Indexed
    {
        OamPrimary::Sprite sprite;
        ScanlineTile scanlineTile;
    };

    /**
     * @brief Holds the tile information for a sprite.
     */
    struct SpritePatternTiles
    {
        /**
         * @brief Number of sprite tiles. Either 1 or 2.
         */
        int numOfTiles;

        /**
         * @brief first tile.
         */
        PatternTables::BitTile firstTile;

        /**
         * @brief second tile. Optional, only for 8 by 16 sprites.
         */
        PatternTables::BitTile secondTile;
    };

    private:
    /**
     * @brief Stores the number of sprites that have been fetched for the next scanline.
     */
    int spriteFetchNum;
    /**
     * @brief Buffer to hold the primary index for sprites fetched for the next scanline.
     */
    std::vector<int> fetchSpritesOamIndex = std::vector<int>(OamPrimary::TotalNumOfSprites);

    /**
     * @brief Stores number of sprites for current scanline.
     */
    int activeSpriteBufferLen;
    /**
     * @brief Buffer to hold the sprites being rendered on the current line.
     */
    std::vector<IndexSpriteBuffer> activeSpriteBuffer = std::vector<IndexSpriteBuffer>(OamPrimary::TotalNumOfSprites);

    public:
    OamSecondary();

    /**
     * @brief Sprites that appear on the next scanline are placed here.
     * @param sprite the sprite to add to the buffer.
     * @param primaryOamIndex the sprite's index in primary OAM.
     */
    void AppendFetchedSprite(const OamPrimary::Sprite& sprite, int primaryOamIndex);

    /**
     * @brief Calculates the scanline tile for a given sprite.
     * @param scanline the scanline number we are rendering.
     * @param sprite the sprite on scanline.
     * @param spriteTile the sprite tile information.
     * @return the scanline extracted from the sprite tile.
     */
    OamSecondary::ScanlineTile CalcSpriteBuffer(int scanline, const OamPrimary::Sprite& sprite, const OamSecondary::SpritePatternTiles& spriteTile) const;
    
    /**
     * @brief Determines the forground pixel value for the given x position on the current scanline.
     * @param pixelX the x position on the scanline.
     * @return pixel pattern.
     */
    OamSecondary::IndexPattern CalcForgroundPixel(int pixelX) const;

    /**
     * @brief Get a fetched sprite.
     * @param spriteNum the secondarySprite index.
     * @return The sprite with the primary index.
     */
    OamSecondary::IndexedSprite GetFetchedSprite(int spriteNum) const;

    /**
     * @brief clear secondary sprite fetch buffer.
     */
    void ClearFetchData();

    /**
     * @brief clear secondary sprite active buffer.
     */
    void ClearActiveBuffer();

    // getters/setters
    int GetSpriteFetchNum() const;
    void AppendToActiveBuffer(const IndexSpriteBuffer& indexedScanline);
};

#endif