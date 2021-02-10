#include "Nes.h"

#include "Cartridge/CartridgeLoader.h"
#include <sstream> //std::stringstream
#include <iostream>

Nes::Nes(int audioSamplesPerSecond)
{
    this->dma = std::make_shared<Dma>();
    this->cpu = std::make_shared<Cpu>(dma);
    this->ppu = std::make_shared<Ppu>(dma);
    this->apu = std::make_shared<Apu>(audioSamplesPerSecond);
}


bool Nes::LoadCartridge(std::string pathToRom)
{
    CartridgeLoader cartridgeLoader;
    std::shared_ptr<ICartridge> cartridge = cartridgeLoader.LoadCartridge(pathToRom);
    if(cartridge == NULL)
    {
        return false;
    }
    bool setCartDma = this->dma->SetCartridge(cartridge);
    if(setCartDma == false)
    {
        return false;
    }
    return true;
}

bool Nes::IsCartridgeLoaded()
{
    return this->dma->GetCartridge() != nullptr;
}

bool Nes::PowerCycle()
{
    bool cputRet = this->cpu->PowerCycle();
    if(cputRet == false)
    {
        return false;
    }

    bool ppuRet = this->ppu->PowerCycle();
    if(ppuRet == false)
    {
        return false;
    }
    
    return true;
}

bool Nes::processFrame(bool verbose)
{
    if(!this->IsCartridgeLoaded())
    {
        //can only process once a ROM has been loaded
        return false;
    }
    // loop until next frame is reached
    long long int frameCount = this->GetFrameCount();
    while( this->GetFrameCount() == frameCount )
    {
        bool stepState = this->Step(verbose);
        if(stepState == false)
        {
            return false;
        }
    }
    return true;
}

const Matrix<rawColour>& Nes::GetFrameDisplay()
{
    return this->ppu->GetFrameDisplay();
}

const long long int& Nes::GetFrameCount() const
{
    return this->ppu->GetTotalFrameCount();
}

bool Nes::PressButton(NesController::NesInputs input, bool isPressed, int controller)
{
    this->dma->GetControllerManager().SetKey(input, isPressed, controller);
    return true;
}

std::shared_ptr<ThreadSafeQueue<float>> Nes::GetAudio()
{
    return this->apu->GetAudio();
}

const std::unique_ptr<Matrix<rawColour>> Nes::GeneratePatternTables()
{
    return this->ppu->GeneratePatternTables();
}

const std::unique_ptr<Matrix<rawColour>> Nes::GenerateColourPalettes()
{
    return this->ppu->GenerateColourPalettes();
}

const std::string Nes::GenerateCpuScreen(int instructionsBefore, int instructionsAfter)
{
    return this->cpu->GenerateCpuScreen(instructionsBefore, instructionsAfter);
}

void Nes::IncrementDefaultColourPalette()
{
    byte palette = this->ppu->GetDisassemblePalette();
    ++palette;
    if(palette >= 8)
    {
        palette = 0;
    }
    this->ppu->SetDisassemblePalette(palette);
}

const std::string Nes::GenerateFirstNameTable()
{
    std::unique_ptr<Matrix<patternIndex>> nameTable = this->dma->GetPpuMemory().GetNameTables().GenerateFirstNameTable();
    std::stringstream ss;
    for(int y = 0; y < nameTable->GetHeight(); ++y)
    {
        for(int x = 0; x < nameTable->GetWidth(); ++x)
        {
            ss << std::hex << (int)nameTable->Get(y, x) << " ";
        }
        std::cout << std::endl;
    }
    return ss.str();
}

const std::unique_ptr<Matrix<rawColour>> Nes::GenerateControllerState( int controller )
{
    const std::vector<bool> keyPresses = this->dma->GetControllerManager().GetKeyPressed(controller);

    /*
    make pixel array like this:
    -----------------
    |*|*|*|*|*|*|*|*|
    -----------------

    "*" = button status
    "|" and "-" are background borders
    */

    rawColour backgroundColour;
    backgroundColour.raw = 0x000000FF;
    rawColour inactiveColour;
    inactiveColour.raw = 0x606060FF;
    rawColour pressColour;
    pressColour.raw = 0xFF0000FF;

    int buttonSize = 4; //pixels
    int borderWidth = 2;
    std::unique_ptr<Matrix<rawColour>> controllerState = std::unique_ptr<Matrix<rawColour>>(new Matrix<rawColour>((8 * buttonSize) + (9 * borderWidth), buttonSize + (2 * borderWidth), backgroundColour));
    
    for(int i = 0; i < 8; ++i)
    {
        int yPos = borderWidth;
        int xPos = i * (buttonSize + borderWidth) + borderWidth;
        if(keyPresses.at(i) == true)
        {
            controllerState->SetRegion(yPos, xPos, buttonSize, buttonSize, pressColour);
        }
        else
        {
            controllerState->SetRegion(yPos, xPos, buttonSize, buttonSize, inactiveColour);
        }
    }

    return controllerState;
}

bool Nes::Step(bool verbose)
{
    int cpuCycles = this->cpu->Step(verbose);

    // run APU for every CPU cycle
    for(int i = 0; i < cpuCycles; ++i)
    {
        this->apu->Step();
    }

    //1 CPU step for 3 PPU steps
    int ppuSteps = cpuCycles * 3;

    // run PPU 3 times for each CPU cycle
    for(int i = 0; i < ppuSteps; ++i)
    {
        this->ppu->Step();

        // handle DMA transfers
        if(this->dma->IsDmaActive())
        {
            this->dma->ProcessDma();
        }
    }
    return true;
}

std::shared_ptr<Dma> Nes::GetDma()
{
    return this->dma;
}