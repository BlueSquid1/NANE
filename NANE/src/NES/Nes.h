#ifndef NES
#define NES

#include <memory> //std::unique_ptr
#include <iostream> //std::string

#include "CPU/Cpu.h"

#include "PPU/Ppu.h"
#include "PPU/PpuRegisters.h"

#include "Cartridge/CartridgeLoader.h"
#include "Cartridge/CartridgeMapping/ICartridge.h"

/**
 * assembles a virtual NES
 */
class Nes
{
    private:
    std::unique_ptr<Cpu> cpu = NULL;
    std::unique_ptr<Ppu> ppu = NULL;
    std::shared_ptr<ICartridge> cartridge = NULL;

    public:
    Nes();

    /**
     * @return false if failed to load .nes ROM
     */
    bool LoadCartridge(std::string pathToRom);

    bool PowerCycle();

    bool processes();

    const Matrix<rawColour>& GetFrameDisplay();

    //dissassmbly methods
    std::unique_ptr<Matrix<rawColour>> GeneratePatternTables();
    std::string GenerateCpuScreen();
};
#endif