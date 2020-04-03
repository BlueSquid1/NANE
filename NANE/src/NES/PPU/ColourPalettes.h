#ifndef COLOUR_PALETTES
#define COLOUR_PALETTES

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/MemoryRepeaterArray.h"
#include "NesColour.h"

/**
 * Manages the PPU colour palettes
 * 
 * https://wiki.nesdev.com/w/index.php/PPU_palettes
 */

struct paletteStruct
{
    union
    {
        colourIndex palettes[8][4];
        struct
        {
            colourIndex backgroundPalettes[4][4];
            colourIndex spritePalettes[4][4];
        };
    };
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

    byte Read(dword address) override;
    void Write(dword address, byte value) override;

    rawColour PatternValueToColour(byte palletteId, byte patternVal) const;

    //getters and setters
    const paletteStruct& GetColourPalettes() const;
};

#endif