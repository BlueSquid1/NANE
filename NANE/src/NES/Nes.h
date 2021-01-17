#pragma once

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
    std::shared_ptr<Dma> dma = std::make_shared<Dma>();
    std::shared_ptr<Cpu> cpu;
    std::shared_ptr<Ppu> ppu;

    public:
    Nes();

    /**
     * @return false if failed to load .nes ROM
     */
    bool LoadCartridge(std::string pathToRom);
    
    bool IsCartridgeLoaded();

    bool PowerCycle();

    bool processFrame(bool verbose);

    const Matrix<rawColour>& GetFrameDisplay();
    const long long int& GetFrameCount() const;

    bool PressButton(NesController::NesInputs input, bool isPressed, int controller = 0);

    //dissassmbly methods
    const std::unique_ptr<Matrix<rawColour>> GeneratePatternTables();
    const std::unique_ptr<Matrix<rawColour>> GenerateColourPalettes();
    const std::string GenerateCpuScreen(int instructionsBefore = 10, int instructionsAfter = 10);
    void IncrementDefaultColourPalette();
    const std::string GenerateFirstNameTable();
    const std::unique_ptr<Matrix<rawColour>> GenerateControllerState(int controller = 0);
    bool Step(bool verbose);
    std::shared_ptr<Dma> GetDma();
};