#include "OamSecondary.h"

#include <exception>

OamSecondary::OamSecondary()
: OamPrimary()
{
    this->Clear();
}

void OamSecondary::AppendSprite(const OamPrimary::Sprite& sprite)
{
    if(this->activeSpriteNum < 0 && this->activeSpriteNum >= OamPrimary::TotalNumOfSprites)
    {
        throw std::invalid_argument("invalid activeSpriteNum to append the current sprite");
    }
    this->name.sprites[this->activeSpriteNum] = sprite;
    ++this->activeSpriteNum;
}

void OamSecondary::PopulateOamTableBuffers(int scanline, int curCycle)
{
    // const Oam::Sprite& sprite = this->name.sprites[i];
    // patternIndex spritePattern = sprite.index;
    // int tableNum = this->GetRegs().name.sprite8x8PatternTable;
    // PatternTables::BitTile& spriteTile = this->dma.GetPatternTile(tableNum, spritePattern);
}

rawColour OamSecondary::CalcForegroundPixel(int curCycle) const
{
    // //loop through secondary OAM
    // OamSecondary& secondaryOam = this->dma.GetPpuMemory().GetSecondaryOam();
    // for(int i = 0; i < secondaryOam.GetActiveSpriteNum(); ++i)
    // {
    //     const Oam::Sprite& sprite = secondaryOam.GetSprite(i);

    // }
    // //look at scanline and compare to secondary OAM X values
    // //calculate first non-transparent pattern value

    // //lookup colour for pattern value (only can use the last 4 palletes)

    // //handle sprite zero hits here (cycle 4)

    // //return colour
    return {0x0};
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

const byte& OamSecondary::GetLsbSpriteTiles(int spriteNum) const
{
    if(spriteNum >= this->activeSpriteNum || spriteNum < 0)
    {
        throw std::invalid_argument("Invalid sprite tile");
    }
    return this->lsbSpriteTiles[spriteNum];
}