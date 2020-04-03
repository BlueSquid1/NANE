#include "Nes.h"

#include "Cartridge/CartridgeLoader.h"

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

bool Nes::processes(bool verbose)
{
    //it takes 29606 cpu cycles to render a whole frame
    int cpuCyclesToRun = 29606;

    if(verbose)
    {
        cpuCyclesToRun = 1;
    }

    int cpuCycles = 0;
    while(cpuCycles < cpuCyclesToRun )
    {
        //1 CPU step for 3 PPU steps
        cpuCycles += this->cpu.Step(verbose);
        for(int i = 0; i < cpuCycles; ++i)
        {
            this->ppu.Step();
            this->ppu.Step();
            this->ppu.Step();
        }
    }
    return true;
}

const Matrix<rawColour>& Nes::GetFrameDisplay()
{
    return this->ppu.GetFrameDisplay();
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