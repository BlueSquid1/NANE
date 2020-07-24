#include "Oam.h"

#include <sstream>
#include <exception>

const int Oam::rawLen;
const int Oam::TotalNumOfSprites;

Oam::Oam()
: MemoryRepeaterArray(0x0, 0x100, this->raw, this->rawLen)
{
    memset(this->raw, 0xFF, sizeof(this->name));
}

std::string Oam::GenerateSpritesProperties() const
{
    std::stringstream ss;
    for(const Sprite& sprite : this->name.sprites)
    {
        ss << "pos:(" << sprite.posX << ", " << sprite.posY << "), index:" << sprite.index << ", attributes:" << sprite.attributes;
    }
    return ss.str();
}

const Oam::Sprite& Oam::GetSprite(int spriteNum) const
{
    if(spriteNum < 0 || spriteNum >= Oam::TotalNumOfSprites)
    {
        throw std::invalid_argument("invalid sprite number to retreive");
    }
    return this->name.sprites[spriteNum];
}