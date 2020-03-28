#ifndef COLOUR_PALETTES
#define COLOUR_PALETTES

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/MemoryRepeaterArray.h"

/**
 * Manages the PPU colour palettes
 * 
 * https://wiki.nesdev.com/w/index.php/PPU_palettes
 */

struct paletteStruct
{
    byte backgroundPalettes[4][4];
    byte spritePalettes[4][4];
};

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

    private:
    /**
     * Some pallete address re-route to the background colour. This method handles those special cases.
     * @param address the address being read/written to
     * @return the same address or in case or rerouting the background colour stored at this->startAddress
     */
    dword Redirect(dword address) const;

    public:
    ColourPalettes();

    byte Read(dword address) const override;
    void Write(dword address, byte value) override;

    //getters and setters
    const paletteStruct& GetColourPalettes() const;
};

#endif