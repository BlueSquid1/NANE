#include "Nes.h"

#include "Cartridge/CartridgeLoader.h"
#include <sstream> //std::stringstream

Nes::Nes()
: dma(), cpu(dma), ppu(dma)
{

}


bool Nes::LoadCartridge(std::string pathToRom)
{
    CartridgeLoader cartridgeLoader;
    std::unique_ptr<ICartridge> cartridge = cartridgeLoader.LoadCartridge(pathToRom);
    if(cartridge == NULL)
    {
        return false;
    }
    bool setCartDma = this->dma.SetCartridge(std::move(cartridge));
    if(setCartDma == false)
    {
        return false;
    }
    return true;
}

bool Nes::PowerCycle()
{
    bool cputRet = this->cpu.PowerCycle();
    if(cputRet == false)
    {
        return false;
    }
    
    return true;
}

bool Nes::processes(bool verbose, bool singleStep)
{
    // loop until next frame is reached
    long long int frameCount = this->GetFrameCount();
    while( this->GetFrameCount() == frameCount )
    {
        //run cpu
        int ppuSteps = 0;
        if(!this->dma.IsDmaActive())
        {
            int cpuCycles = this->cpu.Step(verbose);

            //1 CPU step for 3 PPU steps
            ppuSteps = cpuCycles * 3;
        }
        else
        {
            //cpu skips 1 cpu cycle (= 3 ppu cycles)
            ppuSteps = 3;
        }

        for(int i = 0; i < ppuSteps; ++i)
        {
            this->ppu.Step();

            //handle non-maskable interrupts
            if(this->dma.GetNmi())
            {
                int interruptCycles = this->cpu.HandleNmiEvent(verbose);

                //interupts take time. run PPU during this time
                ppuSteps += interruptCycles * 3;
                this->dma.SetNmi(false);
            }

            // handle DMA transfers
            if(this->dma.IsDmaActive())
            {
                this->dma.ProcessDma();
            }
        }

        // if in single step mode then only run 1 assembly instruction at a time
        if(singleStep)
        {
            return true;
        }
    }
    return true;
}

const Matrix<rawColour>& Nes::GetFrameDisplay()
{
    return this->ppu.GetFrameDisplay();
}

long long int& Nes::GetFrameCount()
{
    return this->ppu.GetTotalFrameCount();
}

bool Nes::PressButton(NesController::NesInputs input, bool isPressed, int controller)
{
    if(controller == 0)
    {
        this->dma.GetControllerPlayer1().SetKey(input, isPressed);
    }
    else if(controller == 1)
    {
        this->dma.GetControllerPlayer2().SetKey(input, isPressed);
    }
    else
    {
        std::cerr << "invalid controller: " << controller << std::endl;
        return false;
    }
    return true;
}

const std::unique_ptr<Matrix<rawColour>> Nes::GeneratePatternTables()
{
    return this->ppu.GeneratePatternTables();
}

const std::unique_ptr<Matrix<rawColour>> Nes::GenerateColourPalettes()
{
    return this->ppu.GenerateColourPalettes();
}

const std::string Nes::GenerateCpuScreen()
{
    return this->cpu.GenerateCpuScreen();
}

void Nes::IncrementDefaultColourPalette()
{
    byte palette = this->ppu.GetDefaultPalette();
    ++palette;
    if(palette >= 8)
    {
        palette = 0;
    }
    this->ppu.SetDefaultPalette(palette);
}

const std::string Nes::GenerateFirstNameTable()
{
    std::unique_ptr<Matrix<patternIndex>> nameTable = this->dma.GetPpuMemory().GetNameTables().GenerateFirstNameTable();
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
    const std::vector<bool> * keyPresses = NULL;
    if(controller == 0)
    {
        keyPresses = &this->dma.GetControllerPlayer1().GetKeyPressed();
    }
    else if(controller == 1)
    {
        keyPresses = &this->dma.GetControllerPlayer2().GetKeyPressed();
    }
    if(keyPresses == NULL)
    {
        std::cerr << "invalid controller: " << controller << std::endl;
        return NULL;
    }
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
        if(keyPresses->at(i) == true)
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