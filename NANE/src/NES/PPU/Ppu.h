#ifndef PPU
#define PPU

#include <memory> //std::unique_ptr

#include "PpuRegisters.h"
#include "NesColour.h"
#include "NES/Memory/BitUtil.h"
#include "NES/Memory/Dma.h"
#include "NES/Memory/Matrix.h"

class Ppu
{
    private:
    struct BackgroundFetchInfo
    {
        byte lsbFetchPattern;
        byte msbFetchPattern;
        paletteIndex paletteColour : 4;
    };

    /**
     * @brief Stores background pixel info.
     */
    struct BackgroundPixelInfo
    {
        bool isTransparent; //true: anything behind the background will be drawn instead of the background colour. False: background is solid.
        rawColour pixelColour; //background colour
    };

    /**
     * @brief Stores foreground pixel info.
     */
    struct ForegroundPixelInfo : public Ppu::BackgroundPixelInfo
    {
        bool frontOfBackground; //true: sprite should be drawn infront of the background
        byte primaryOamIndex; //the sprite primary OAM index
    };

    /**
     * special PPU cycles and scanline numbers
     */
    const int PRE_SCANLINE = -1; //scanline
    const int START_VISIBLE_SCANLINE = 0; //scanlines
    const int LAST_VISIBLE_SCANLINE = 239; //scanlines
    const int VBLANK_SCANLINE = 241;
    const int LAST_SCANLINE = 260; //scanlines

    const int START_CYCLE = 0;
    const int START_VISIBLE_CYCLE = 1; //cycles
    const int START_SPRITE_EVALUATION_CYCLE = 65; //cycles
    const int LAST_VISIBLE_FETCH_CYCLE = 256 - 8; //cycles
    const int LAST_VISIBLE_CYCLE = 256; //cycles
    const int START_SPRITE_TILE_FETCH_CYCLE = 261; //cycles
    const int START_NEXT_SCANLINE_FETCHING_CYCLE = 321; //cycles
    const int LAST_NEXT_SCANLINE_FETCHING_CYCLE = 336; //cycles
    const int LAST_CYCLE = 340; //cycles

    /**
     * @brief Reference of NES memory bus.
     */
    Dma& dma;

    /**
     * @brief Matrix of output pixel colours.
     * width (cycles)  x  height (scanlines)
     *      256        x       240
     */
    Matrix<rawColour> framebuffer;

    /**
     * @brief Frame count - how many frames have been rendered.
     */
    long long int frameCountNum;

        /**
     * @brief Disassemble colour palette to use.
     */
    byte disassemblePalette = 0;

    /**
     * @brief Calculates the next cycle and scanline that needs to be processed.
     * @param curCycle the current cycle.
     * @param curLine the current scanline.
     * @return the next scanline and cycle for the PPU.
     */
    Point calcNextPosition(int curCycle, int curLine);

    /**
     * @brief Calculates the next fetch pixel for the current current cycle and scanline.
     * @param curCycle the current cycle.
     * @param curLine the current scanline.
     * @return the x and y position of the pixel that needs to be fetched.
     * Note: 
     * 0 <= x <= 255
     * 0 <= y <= 239
     */
    Point CalcNextFetchPixel(int curCycle, int curLine);

    /**
     * @brief Handles populating the background buffer.
     * @param fetchTile the tile to fetch to fill the background buffer.
     * @param curCycle the current cycle.
     * @param curLine the current scanline.
     */
    std::unique_ptr<Ppu::BackgroundFetchInfo> backgroundFetch(int curCycle, int curLine);

    /**
     * @brief Handles populating the forground buffer.
     * @param curCycle the current cycle.
     * @param curLine the current scanline.
     */
    void SpriteFetch(int curCycle, int curLine);

    /**
     * @brief Calculates the background pixel from the working background buffer.
     * @param curCycle the current cycle.
     * @return background colour and whether or not the background is invisible.
     */
    Ppu::BackgroundPixelInfo CalcBackgroundPixel(int curCycle, const PpuRegisters::VirtualRegisters::BackgroundDrawRegisters& bDrawingRegs);

    /**
     * @brief calculates the forground pixel.
     * @param curCycle the current cycle.
     * @return sprite colour, front of background, and primary sprite OAM index (set to -1 if no sprite).
     */
    Ppu::ForegroundPixelInfo CalcForgroundPixel(int curCycle);

    /**
     * @brief Picks between the background and foreground pixel.
     * @param bPixel background pixel info.
     * @param sPixel forground pixel info.
     * @param curCycle current cycle.
     * @return the final colour to draw on the screen.
     */
    rawColour CalcFinalPixel(const Ppu::BackgroundPixelInfo& bPixel, const Ppu::ForegroundPixelInfo& sPixel, int curCycle);

    /**
     * @brief Getter for PPU registers.
     * @return PPU registers.
     */
    PpuRegisters& GetRegs();

    public:
    //constructor
    Ppu(Dma& dma);

    /**
     * @brief Trigger a power reset for the PPU.
     * @return true is successfully.
     */
    bool PowerCycle();

    /**
     * @brief simulates 1 PPU cycle.
     */
    void Step();

    /**
     * @brief Retreives the main output display.
     * @return the main output screen.
     */
    const Matrix<rawColour>& GetFrameDisplay();

    /**
     * just for dissassembly purposes
     */
    std::unique_ptr<Matrix<rawColour>> GeneratePatternTables();
    std::unique_ptr<Matrix<rawColour>> GenerateColourPalettes();
    byte GetDisassemblePalette() const;
    void SetDisassemblePalette(byte palette);
    const long long int& GetTotalFrameCount() const;
};

#endif