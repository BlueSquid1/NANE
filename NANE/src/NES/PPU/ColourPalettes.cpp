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
    byte colourIndex = this->Seek(this->startAddress + 4 * palletteId + patternVal);
    return NesColour::GetRawColour(colourIndex);
}


std::unique_ptr<Matrix<rawColour>> ColourPalettes::GenerateColourPalettes(byte disassemblePalette)
{
    const int colourWidth = 3;
    const int borderWidth = 2;
    rawColour backgroundColour;
    backgroundColour.raw = 0x000000FF;

    std::unique_ptr<Matrix<rawColour>> colourOutput = std::unique_ptr<Matrix<rawColour>>( new Matrix<rawColour>(borderWidth * 9 + colourWidth * 8, borderWidth + colourWidth * 4 + borderWidth, backgroundColour) );

    int curCol = borderWidth;
    for(int paletteId = 0; paletteId < 8; ++paletteId)
    {
        if(paletteId == disassemblePalette)
        {
            rawColour selectedColour = {0xBBBBBBFF};
            colourOutput->SetRegion(0, curCol - borderWidth, colourWidth + 2 * borderWidth, colourOutput->GetHeight(), selectedColour);
        }
        for(int colourNum = 0; colourNum < 4; ++colourNum)
        {
            rawColour colour = this->PatternValueToColour(paletteId, colourNum);
            colourOutput->SetRegion(borderWidth + (colourWidth * colourNum), curCol, colourWidth, colourWidth, colour);
        }
        curCol += colourWidth;
        curCol += borderWidth;
    }

    return colourOutput;
}