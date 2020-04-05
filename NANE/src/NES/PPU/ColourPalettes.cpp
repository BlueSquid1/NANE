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
    switch(offset)
    {
        case 0x10:
        redirrectAddress = 0x3F00;
        break;
        case 0x14:
        redirrectAddress = 0x3F04;
        break;
        case 0x18:
        redirrectAddress = 0x3F08;
        break;
        case 0x1C:
        redirrectAddress = 0x3F0C;
        break;
    }
    return redirrectAddress;
}

byte ColourPalettes::Read(dword address)
{
    return this->Seek(address);
}

void ColourPalettes::Write(dword address, byte value)
{
    dword redirrectAddress = this->Redirect(address);
    return MemoryRepeaterArray::Write(redirrectAddress, value);
}

byte ColourPalettes::Seek(dword address) const
{
    dword redirrectAddress = this->Redirect(address);
    return MemoryRepeaterArray::Seek(redirrectAddress);
}
#include <iostream>
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
    byte colourIndex = this->Seek(0x3F00 + 4 * palletteId + patternVal);
    return NesColour::GetRawColour(colourIndex);
}