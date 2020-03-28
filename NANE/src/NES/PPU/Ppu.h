#ifndef PPU
#define PPU

#include <memory> //std::shared_ptr

#include "Screen.h"
#include "NesColour.h"
#include "PpuRegisters.h"
#include "NES/APU/ApuRegisters.h"
#include "PpuMemoryMap.h"
#include "NES/Memory/BitUtil.h"

class Ppu
{
    private:
    PpuMemoryMap ppuMemory;

    //the actual output
    // length  x  scanlines
    //  256    x   240
    Screen framebuffer;

    long long int frameCountNum; //how many frames have been rendered

    rawColour calc_background_pixel();
    rawColour calc_sprite_pixel();

    void background_fetch();
    void sprite_fetch();

    public:
    //constructor
    Ppu(std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters);

    bool PowerCycle();
    int Step();

    bool SetCartridge(std::shared_ptr<ICartridge> cartridge);
    
    const Screen& GetFrameDisplay();
    const Screen& GetChrRom();
};

#endif