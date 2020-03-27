#ifndef PPU
#define PPU

#include <memory> //std::shared_ptr

#include "NesColour.h"
#include "PpuRegisters.h"
#include "NES/APU/ApuRegisters.h"
#include "PpuMemoryMap.h"
#include "NES/Memory/BitUtil.h"

class Ppu
{
    private:
    std::unique_ptr<PpuMemoryMap> ppuMemory = NULL;

    //the actual output
    // length  x  scanlines
    //  256    x   240
    std::shared_ptr<std::vector<NesColour>> framebuffer = NULL;

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