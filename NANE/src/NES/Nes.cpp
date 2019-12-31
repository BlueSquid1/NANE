#include "Nes.h"

Nes::Nes()
{
    std::unique_ptr<CpuRegisters> cpuRegisters( new CpuRegisters() );
    std::shared_ptr<PpuRegisters> ppuRegisters( new PpuRegisters() );
    std::shared_ptr<ApuRegisters> apuRegisters( new ApuRegisters() );

    this->cpu = std::unique_ptr<Cpu>( new Cpu(std::move(cpuRegisters), ppuRegisters, apuRegisters) );
    this->ppu = std::unique_ptr<Ppu>( new Ppu(ppuRegisters) );
}


bool Nes::LoadCartridge(std::string pathToRom)
{
    CartridgeLoader cartridgeLoader;
    this->cartridge = cartridgeLoader.LoadCartridge(pathToRom);
    if(this->cartridge == NULL)
    {
        return false;
    }
    this->cpu->SetCartridge(this->cartridge);

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
    this->cpu->Step();
    return true;
}