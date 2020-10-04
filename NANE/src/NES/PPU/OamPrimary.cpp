#include "OamPrimary.h"

#include <sstream>
#include <exception>

const int OamPrimary::rawLen;
const int OamPrimary::TotalNumOfSprites;

OamPrimary::OamPrimary()
: MemoryRepeaterArray(0x0, 0x100, this->raw, this->rawLen)
{
    memset(this->raw, 0xFF, sizeof(this->name));
}

const OamPrimary::Sprite& OamPrimary::GetSprite(int spriteNum) const
{
    if(spriteNum < 0 || spriteNum >= OamPrimary::TotalNumOfSprites)
    {
        throw std::invalid_argument("invalid sprite number to retreive");
    }
    return this->name.sprites[spriteNum];
}