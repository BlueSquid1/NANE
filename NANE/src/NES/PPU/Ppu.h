#ifndef PPU
#define PPU

#include "PpuRegisters.h"
#include "Matrix.h"
#include "NesColour.h"
#include "NES/Memory/BitUtil.h"
#include "NES/Memory/Dma.h"

#include <memory> //std::unique_ptr

class Ppu
{
    private:
    Dma& dma;

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

    PpuRegisters& GetRegs();

    public:
    //constructor
    Ppu(Dma& dma);

    bool PowerCycle();
    int Step();

    /**
     * just for dissassembly purposes
     */
    std::unique_ptr<Matrix<rawColour>> GeneratePatternTables();

    std::unique_ptr<Matrix<rawColour>> GenerateColourPalettes();

    const Matrix<rawColour>& GetFrameDisplay();

    //getters/setters
    byte GetDefaultPalette() const;
    void SetDefaultPalette(byte palette);
};

#endif