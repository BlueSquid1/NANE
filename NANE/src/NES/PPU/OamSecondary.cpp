#include "OamSecondary.h"

#include "PatternTables.h"

#include <exception>

OamSecondary::OamSecondary()
: OamPrimary()
{
    this->ClearFetchData();
    this->ClearActiveBuffer();
}

void OamSecondary::AppendFetchedSprite(const OamPrimary::Sprite& sprite, int primaryOamIndex)
{
    this->name.sprites[this->spriteFetchNum] = sprite;
    this->fetchSpritesOamIndex.at(this->spriteFetchNum) = primaryOamIndex;
    ++this->spriteFetchNum;
}

OamSecondary::ScanlineTile OamSecondary::CalcSpriteBuffer(int scanline, const OamPrimary::Sprite& sprite, const OamSecondary::SpritePatternTiles& spriteTiles) const
{
    int tileLine = scanline - sprite.posY;

    // handle vertical flipping
    const PatternTables::BitTile* activeSpriteTile = nullptr;
    if(spriteTiles.numOfTiles == 1)
    {
        // standard 8 x 8 sprite
        if(sprite.flipVertically)
        {
            tileLine = 7 - tileLine;
        }
        activeSpriteTile = &spriteTiles.firstTile;
    }
    else
    {
        // large 8 x 16 sprite
        if(sprite.flipVertically)
        {
            tileLine = 15 - tileLine;
        }

        if(tileLine < 8)
        {
            activeSpriteTile = &spriteTiles.firstTile;
        }
        else
        {
            activeSpriteTile = &spriteTiles.secondTile;
            tileLine = tileLine % PatternTables::TILE_HEIGHT;
        }
    }

    if(tileLine < 0 || tileLine >= PatternTables::TILE_HEIGHT)
    {
        throw std::invalid_argument("scanline does not contain the sprite");
    }

    OamSecondary::ScanlineTile spriteBuffer;
    spriteBuffer.lsbSpriteTile = activeSpriteTile->lsbPlane[tileLine];
    spriteBuffer.msbSpriteTile = activeSpriteTile->msbPlane[tileLine];

    // because number are little endian numbers are flip horizontally by default. unflip is necassary
    if(!sprite.flipHorizontally)
    {
        spriteBuffer.lsbSpriteTile = BitUtil::FlipByte(spriteBuffer.lsbSpriteTile);
        spriteBuffer.msbSpriteTile = BitUtil::FlipByte(spriteBuffer.msbSpriteTile);
    }
    return spriteBuffer;
}

OamSecondary::IndexPattern OamSecondary::CalcForgroundPixel(int pixelX) const
{
    // loop through secondary OAM and return the first non transparent sprite
    for(int i = 0; i < this->activeSpriteBufferLen; ++i)
    {
        const IndexSpriteBuffer& spriteBuffer = this->activeSpriteBuffer.at(i);
        //check if the cycle is within the sprite width
        int spriteOffset = pixelX - spriteBuffer.sprite.posX;
        if(spriteOffset >= 0 && spriteOffset < PatternTables::TILE_WIDTH)
        {
            //check if the pixel is transparent (i.e. pattern = 0)
            const ScanlineTile& scanlineBuffer = spriteBuffer.scanlineTile;
            bit lsbSpriteTile = BitUtil::GetBits(scanlineBuffer.lsbSpriteTile, spriteOffset);
            bit msbSpriteTile = BitUtil::GetBits(scanlineBuffer.msbSpriteTile, spriteOffset);
            patternIndex patternValue = (msbSpriteTile << 1) | lsbSpriteTile;
            if(patternValue != 0)
            {
                IndexPattern forgroundPixel;
                forgroundPixel.pattern = patternValue;
                forgroundPixel.sprite = spriteBuffer.sprite;
                forgroundPixel.primaryOamIndex = spriteBuffer.primaryOamIndex;
                return forgroundPixel;
            }
        }
    }
    //no sprite at current PPU position
    IndexPattern noForgroundPixel;
    noForgroundPixel.primaryOamIndex = -1;
    return noForgroundPixel;
}

OamSecondary::IndexedSprite OamSecondary::GetFetchedSprite(int spriteNum) const
{
    if(spriteNum < 0 || spriteNum >= this->spriteFetchNum)
    {
        throw std::invalid_argument("invalid sprite number");
    }
    OamSecondary::IndexedSprite indexedSprite;
    indexedSprite.sprite = this->GetSprite(spriteNum);
    indexedSprite.primaryOamIndex = this->fetchSpritesOamIndex.at(spriteNum);
    return indexedSprite;
}

void OamSecondary::ClearFetchData()
{
    // this hinted at that fact that OAM is cleared to 0xFF instead of 0x00:
    // https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation#Details
    memset(this->raw, 0xFF, sizeof(this->name));

    this->spriteFetchNum = 0;
}

void OamSecondary::ClearActiveBuffer()
{
    this->activeSpriteBufferLen = 0;
}

int OamSecondary::GetSpriteFetchNum() const
{
    return this->spriteFetchNum;
}

void OamSecondary::AppendToActiveBuffer(const IndexSpriteBuffer& indexedScanline)
{
    this->activeSpriteBuffer.at(this->activeSpriteBufferLen) = indexedScanline;
    ++this->activeSpriteBufferLen;
}