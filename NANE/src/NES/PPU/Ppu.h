#ifndef PPU
#define PPU

#include "PpuRegisters.h"
#include "NesColour.h"
#include "NES/Memory/BitUtil.h"
#include "NES/Memory/Dma.h"
#include "NES/Memory/Matrix.h"

#include <memory> //std::unique_ptr

class Ppu
{
    private:
    struct Point
    {
        int x;
        int y;
    };
    const int PRE_SCANLINE = -1; //scanline
    const int START_VISIBLE_SCANLINE = 0; //scanlines
    const int LAST_VISIBLE_SCANLINE = 239; //scanlines
    const int LAST_SCANLINE = 260; //scanlines
    const int LAST_CYCLE = 340; //scanlines

    const int START_VISIBLE_CYCLE = 1; //cycles
    const int START_SPRITE_EVALUATION_CYCLE = 65; //cycles
    const int LAST_VISIBLE_FETCH_CYCLE = 256 - 8; //cycles
    const int LAST_VISIBLE_CYCLE = 256; //cycles
    const int START_SPRITE_TILE_FETCH = 261;
    const int START_NEXT_SCANLINE_FETCHING = 321; //cycles
    const int LAST_NEXT_SCANLINE_FETCHING = 336; //cycles
    const int FIRST_NON_VISIBLE_CYCLE = 257; // cycles

    Dma& dma;

    //the actual output
    // length  x  scanlines
    //  256    x   240
    Matrix<rawColour> framebuffer;

    long long int frameCountNum; //how many frames have been rendered
    byte defaultPalette = 0;

    Point NextPixel();

    rawColour calc_background_pixel();

    void backgroundFetch(std::unique_ptr<Ppu::Point>& fetchTile, int curCycle, int curLine);
    void sprite_fetch(int curCycle, int curLine);

    PpuRegisters& GetRegs();

    public:
    //constructor
    Ppu(Dma& dma);

    bool PowerCycle();
    int Step();

    std::unique_ptr<Ppu::Point> CalcNextBgrFetchTile(int curCycle, int curLine);

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