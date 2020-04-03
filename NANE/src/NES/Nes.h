#ifndef NES
#define NES

#include "CPU/Cpu.h"
#include "PPU/Ppu.h"
#include "Memory/Dma.h"

#include <memory> //std::unique_ptr
#include <string> //std::string

/**
 * assembles a virtual NES
 */
class Nes
{
    private:
    Dma dma;
    Cpu cpu;
    Ppu ppu;

    public:
    Nes();

    /**
     * @return false if failed to load .nes ROM
     */
    bool LoadCartridge(std::string pathToRom);

    bool PowerCycle();

    bool processes(bool verbose);

    const Matrix<rawColour>& GetFrameDisplay();

    //dissassmbly methods
    const std::unique_ptr<Matrix<rawColour>> GeneratePatternTables();
    const std::unique_ptr<Matrix<rawColour>> GenerateColourPalettes();
    const std::string GenerateCpuScreen();
};
#endif