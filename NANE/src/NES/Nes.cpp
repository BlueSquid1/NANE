#include "Nes.h"

Nes::Nes()
{
    std::shared_ptr<PpuRegisters> ppuRegisters( new PpuRegisters() );
    std::shared_ptr<ApuRegisters> apuRegisters( new ApuRegisters() );

    this->cpu = std::unique_ptr<Cpu>( new Cpu(ppuRegisters, apuRegisters) );
    this->ppu = std::unique_ptr<Ppu>( new Ppu(ppuRegisters, apuRegisters) );
}


bool Nes::LoadCartridge(std::string pathToRom)
{
    CartridgeLoader cartridgeLoader;
    this->cartridge = cartridgeLoader.LoadCartridge(pathToRom);
    if(this->cartridge == NULL)
    {
        return false;
    }
    bool setCartRet = this->cpu->SetCartridge(this->cartridge);
    if(setCartRet == false)
    {
        return false;
    }

    return true;
}

bool Nes::PowerCycle()
{
    bool cputRet = this->cpu->PowerCycle();
    if(cputRet == false)
    {
        return false;
    }
    
    return true;
}

bool Nes::processes()
{
    //1 CPU step for 3 PPU steps
    int cpuCycles = this->cpu->Step();
    return true;
}