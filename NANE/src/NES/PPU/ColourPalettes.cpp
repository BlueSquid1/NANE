#include "ColourPalettes.h"

const int ColourPalettes::rawLen;

ColourPalettes::ColourPalettes()
: MemoryRepeaterArray(0x3F00, 0x3FFF, this->raw, this->rawLen)
{
    memset(this->raw, 0, this->rawLen);
}

dword ColourPalettes::Redirect(dword address) const
{
    dword redirrectAddress = address;
    dword offset = this->LowerOffset(address);
    if(offset % 4 == 0)
    {
        redirrectAddress = this->startAddress;
    }
    return redirrectAddress;
}

byte ColourPalettes::Read(dword address) const
{
    dword redirrectAddress = this->Redirect(address);
    return MemoryRepeaterArray::Read(redirrectAddress);
}

void ColourPalettes::Write(dword address, byte value)
{
    dword redirrectAddress = this->Redirect(address);
    return MemoryRepeaterArray::Write(redirrectAddress, value);
}

rawColour ColourPalettes::PatternValueToColour(byte palletteId, byte patternVal) const
{
    if(palletteId < 0 || palletteId >= 8)
    {
        throw std::invalid_argument("palletteId is outside boundary for a ColourPalette");
    }
    else if(patternVal < 0 || patternVal >= 4)
    {
        throw std::invalid_argument("patternVal is outside boundary for a ColourPalette");
    }
    colourIndex colourIndex = this->name.palettes[palletteId][patternVal];
    return NesColour::GetRawColour(colourIndex);
}

const paletteStruct& ColourPalettes::GetColourPalettes() const
{
    return this->name;
}