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
    struct Point
    {
        int x;
        int y;
    };
    const int LAST_CYCLE = 340;
    const int LAST_SCANLINE = 260;

    const int START_NEXT_SCANLINE_FETCHING = 321; //cycles
    const int LAST_NEXT_SCANLINE_FETCHING = 336; //cycles
    const int START_VISIBLE_CYCLE = 1; //cycles
    const int LAST_VISIBLE_CYCLE = 256; //cycles
    const int LAST_VISIBLE_FETCH_CYCLE = 256 - 8;
    const int PRE_SCANLINE = -1;
    const int START_VISIBLE_SCANLINE = 0; //scanlines
    const int LAST_VISIBLE_SCANLINE = 239; //scanlines

    Dma& dma;

    //the actual output
    // length  x  scanlines
    //  256    x   240
    Matrix<rawColour> framebuffer;

    long long int frameCountNum; //how many frames have been rendered
    byte defaultPalette = 0;

    Point NextPixel();

    rawColour calc_background_pixel();
    rawColour calc_sprite_pixel();

    void backgroundFetch(std::unique_ptr<Ppu::Point>& fetchTile);
    void sprite_fetch();

    PpuRegisters& GetRegs();

    public:
    //constructor
    Ppu(Dma& dma);

    bool PowerCycle();
    int Step();

    std::unique_ptr<Ppu::Point> CalcNextFetchTile();

    void ProcessDma();

    bool IsDmaActive();

    /**
     * just for dissassembly purposes
     */
    std::unique_ptr<Matrix<rawColour>> GeneratePatternTables();

    std::unique_ptr<Matrix<rawColour>> GenerateColourPalettes();

    const Matrix<rawColour>& GetFrameDisplay();

    //getters/setters
    byte GetDefaultPalette() const;
    void SetDefaultPalette(byte palette);
    long long int& GetTotalFrameCount();
};

#endif