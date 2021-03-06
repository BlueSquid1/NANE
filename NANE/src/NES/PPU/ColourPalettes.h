#pragma once

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/MemoryRepeaterArray.h"
#include "NES/Memory/Matrix.h"

#include "NesColour.h"

/**
 * Manages the PPU colour palettes
 * 
 * https://wiki.nesdev.com/w/index.php/PPU_palettes
 */

#pragma pack(push, 1)
struct paletteStruct
{
    colourIndex backgroundPalettes[4][4];
    /*
    Note: the first index for each sprite palette can be considered 
    'transparent' as is redirrects to the background colour.
    */
    colourIndex spritePalettes[4][4];
};
#pragma pack(pop)

class ColourPalettes : public MemoryRepeaterArray
{
    private:
    static const int rawLen = 32;
    //anonymous union
    union
    {
        paletteStruct name;
        byte raw[rawLen];
    };

    /**
     * Some pallete address re-route to the background colour. This method handles those special cases.
     * @param address the address being read/written to
     * @return the same address or in case or rerouting the background colour stored at this->startAddress
     */
    dword Redirect(dword address) const;

    public:
    ColourPalettes();

    byte Read(dword address) override;
    void Write(dword address, byte value) override;
    byte Seek(dword address) const override;

    rawColour PatternValueToColour(byte palletteId, byte patternVal) const;

    // Disassemble methods
    std::unique_ptr<Matrix<rawColour>> GenerateColourPalettes(byte disassemblePalette);
};