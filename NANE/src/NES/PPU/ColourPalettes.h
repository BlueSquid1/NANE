#ifndef COLOUR_PALETTES
#define COLOUR_PALETTES

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/MemoryRepeaterArray.h"

/**
 * Manages the PPU colour palettes
 * 
 * https://wiki.nesdev.com/w/index.php/PPU_palettes
 */
class ColourPalettes : MemoryRepeaterArray
{
    private:
    dword Redirect(dword address) const;

    public:
    struct paletteStruct
    {
        byte backgroundPalette[4][4];
        struct
        {
            byte backgroundMirror;
            byte palette;
        }spritePalette[4];
    };

    //anonymous union
    union
    {
        paletteStruct name;
        byte raw[sizeof(paletteStruct)];
    };
    const int rawLen = sizeof(paletteStruct);

    ColourPalettes(dword startAddress, dword endAddress);

    byte Read(dword address) const override;
    void Write(dword address, byte value) override;
};

#endif