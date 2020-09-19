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
    
    bool IsCartridgeLoaded();

    bool PowerCycle();

    bool processes(bool verbose, bool singleStep = false);

    const Matrix<rawColour>& GetFrameDisplay();
    const long long int& GetFrameCount() const;

    bool PressButton(NesController::NesInputs input, bool isPressed, int controller = 0);

    //dissassmbly methods
    const std::unique_ptr<Matrix<rawColour>> GeneratePatternTables();
    const std::unique_ptr<Matrix<rawColour>> GenerateColourPalettes();
    const std::string GenerateCpuScreen();
    void IncrementDefaultColourPalette();
    const std::string GenerateFirstNameTable();
    const std::unique_ptr<Matrix<rawColour>> GenerateControllerState(int controller = 0);
};
#endif