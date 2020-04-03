#ifndef PPU
#define PPU

#include <memory> //std::shared_ptr

#include "Matrix.h"
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
    Matrix<rawColour> framebuffer;

    long long int frameCountNum; //how many frames have been rendered
    byte defaultPalette = 0;

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

    /**
     * just for dissassembly purposes
     */
    std::unique_ptr<Matrix<rawColour>> GeneratePatternTables();

    std::unique_ptr<Matrix<rawColour>> GenerateColourPalettes();

    const Matrix<rawColour>& GetFrameDisplay();
};

#endif