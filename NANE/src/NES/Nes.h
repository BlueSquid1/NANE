#pragma once

#include <memory> //std::unique_ptr
#include <string> //std::string

#include "Memory/ThreadSafeQueue.h"
#include "Memory/Matrix.h"

#include "CPU/Cpu.h"
#include "PPU/Ppu.h"
#include "Memory/Dma.h"
#include "APU/Apu.h"

/**
 * assembles a virtual NES
 */
class Nes
{
    private:
    std::shared_ptr<Dma> dma = std::make_shared<Dma>();
    std::shared_ptr<Cpu> cpu;
    std::shared_ptr<Ppu> ppu;
    std::shared_ptr<Apu> apu;

    public:
    Nes(int audioSamplesPerSecond);

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

    std::shared_ptr<ThreadSafeQueue<float>> GetAudio();

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