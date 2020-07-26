#include "OamSecondary.h"

#include "PatternTables.h"

#include <exception>

OamSecondary::OamSecondary()
: OamPrimary()
{
    this->Clear();
}

void OamSecondary::AppendSprite(const OamPrimary::Sprite& sprite, int primaryOamIndex)
{
    if(this->activeSpriteNum < 0 && this->activeSpriteNum >= OamPrimary::TotalNumOfSprites)
    {
        throw std::invalid_argument("invalid activeSpriteNum to append the current sprite");
    }
    this->name.sprites[this->activeSpriteNum] = sprite;
    this->SpriteBuffers[this->activeSpriteNum].primaryOamIndex = primaryOamIndex;
    ++this->activeSpriteNum;
}

OamSecondary::ScanlineTile OamSecondary::CalcSpriteBuffer(int scanline, const OamPrimary::Sprite& sprite, const OamSecondary::SpritePatternTiles& spriteTiles) const
{
    int tileLine = scanline - sprite.posY;
    if(tileLine < 0 || tileLine > (PatternTables::TILE_HEIGHT * spriteTiles.numOfTiles))
    {
        throw std::invalid_argument("scanline does not contain the sprite");
    }

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
        }
    }

    OamSecondary::ScanlineTile spriteBuffer;
    spriteBuffer.lsbSpriteTile = activeSpriteTile->LsbPlane[tileLine];
    spriteBuffer.msbSpriteTile = activeSpriteTile->MsbPlane[tileLine];

    if(!sprite.flipHorizontally)
    {
        spriteBuffer.lsbSpriteTile = BitUtil::FlipByte(spriteBuffer.lsbSpriteTile);
        spriteBuffer.msbSpriteTile = BitUtil::FlipByte(spriteBuffer.msbSpriteTile);
    }
    return spriteBuffer;
}

OamSecondary::SpritePixel OamSecondary::CalcForgroundPixel(int curCycle) const
{
    // loop through secondary OAM and return the first non transparent sprite
    for(int i = 0; i < this->activeSpriteNum; ++i)
    {
        const Sprite& sprite = this->GetSprite(i);
        //check if the cycle is within the sprite width
        int spriteOffset = curCycle - sprite.posX - 1;
        if(spriteOffset >= 0 && spriteOffset < PatternTables::TILE_WIDTH)
        {
            //check if the pixel is transparent (i.e. pattern = 0)
            const ScanlineTile& spriteBuffer = this->GetSpriteScanlineTile(i);
            bit lsbSpriteTile = BitUtil::GetBits(spriteBuffer.lsbSpriteTile, spriteOffset);
            bit msbSpriteTile = BitUtil::GetBits(spriteBuffer.msbSpriteTile, spriteOffset);
            patternIndex patternValue = (msbSpriteTile << 1) | lsbSpriteTile;
            if(patternValue != 0)
            {
                SpritePixel spritePixel;
                spritePixel.primaryOamIndex = i;
                spritePixel.pattern = patternValue;
                return spritePixel;
            }
        }
    }
    //no sprite at current PPU position
    SpritePixel spritePixel;
    spritePixel.primaryOamIndex = -1;
    return spritePixel;
}

void OamSecondary::Clear()
{
    // this hinted at that fact that OAM is cleared to 0xFF instead of 0x00:
    // https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation#Details
    memset(this->raw, 0xFF, sizeof(this->name));

    this->activeSpriteNum = 0;
}

int OamSecondary::GetActiveSpriteNum() const
{
    return this->activeSpriteNum;
}

const OamSecondary::ScanlineTile& OamSecondary::GetSpriteScanlineTile(int spriteNum) const
{
    if(spriteNum >= this->activeSpriteNum || spriteNum < 0)
    {
        throw std::invalid_argument("Invalid sprite tile");
    }
    return this->SpriteBuffers[spriteNum].scanlineTile;
}

void OamSecondary::SetSpriteScanlineTile(int spriteNum, const OamSecondary::ScanlineTile& spriteBuffer)
{
    if(spriteNum >= this->activeSpriteNum || spriteNum < 0)
    {
        throw std::invalid_argument("Invalid sprite tile");
    }
    this->SpriteBuffers[spriteNum].scanlineTile = spriteBuffer;
}