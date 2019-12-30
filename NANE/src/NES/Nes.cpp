#include "Nes.h"

Nes::Nes()
{
    //init

    //load ROM
    INesParser iNesParser;
    std::unique_ptr<INes> ines = iNesParser.ParseINes("some .nes");
    std::shared_ptr<ICartridge> cartridge = iNesParser.GenerateCartridge(std::move(ines));

    std::unique_ptr<CpuRegisters> cpuRegisters( new CpuRegisters() );
    std::shared_ptr<PpuRegisters> ppuRegisters( new PpuRegisters() );
    std::shared_ptr<ApuRegisters> apuRegisters( new ApuRegisters() );


    std::unique_ptr<CpuMemoryMap> cpuMemoryMap( new CpuMemoryMap(ppuRegisters, apuRegisters, cartridge) );
    std::shared_ptr<Cpu> cpu( new Cpu( std::move(cpuRegisters), std::move(cpuMemoryMap) ) );
}