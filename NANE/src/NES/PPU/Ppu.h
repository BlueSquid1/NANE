#ifndef PPU
#define PPU

#include <memory> //std::shared_ptr

#include "NesColour.h"
#include "PpuRegisters.h"
#include "NES/APU/ApuRegisters.h"
#include "PpuMemoryMap.h"
#include "NES/Memory/BitUtil.h"
#include "ColourPalettes.h"

class Ppu
{
    private:
    std::shared_ptr<PpuRegisters> registers = NULL;
    std::shared_ptr<ApuRegisters> apuRegisters = NULL;
    
    std::shared_ptr<ColourPalettes> palettes = NULL;

    std::unique_ptr<std::vector<byte>> primOam = NULL; //256 bytes
    std::unique_ptr<std::vector<byte>> secOam = NULL; //32 bytes (8 sprites to render on scanline)

    std::unique_ptr<PpuMemoryMap> memory = NULL;

    //the actual output
    // length  x  scanlines
    //  256    x   240
    std::shared_ptr<std::vector<NesColour>> framebuffer = NULL;

    dword curBackPatternPlane1; //background pattern plane 1 (controls bit 0)
    dword curBackPatternPlane2; //background pattern plane 2 (controls bit 1)

    byte curBackPaletteAttribute1;
    byte curBackPaletteAttribute2;

    //titleLatches

    int scanlineNum = -1; //the current scanline being rendered between -1 and 260
    int scanCycleNum = 0; //the cycle num for the current scanline between 0 and 340

    long long int frameCountNum; //how many frames have been rendered

    NesColour calc_background_pixel();
    NesColour calc_sprite_pixel();

    void background_fetch();
    void sprite_fetch();

    public:
    //constructor
    Ppu(std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters);

    bool PowerCycle();
    int Step();
    
    std::shared_ptr<std::vector<NesColour>> GetFrameDisplay() const;
};

#endif