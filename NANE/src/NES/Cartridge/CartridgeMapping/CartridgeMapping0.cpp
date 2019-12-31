#include "CartridgeMapping0.h"

CartridgeMapping0::CartridgeMapping0()
: ICartridge(0)
{

}

bool CartridgeMapping0::LoadINes(std::unique_ptr<INes> INesRom)
{
    this->prgRom = std::unique_ptr<MemoryRepeater>( new MemoryRepeater(0x8000, 0xFFFF, INesRom->GetPrgRomData()) );

    if(INesRom->GetBatteryPresent())
    {
        dword ramSize = 8192;
        std::unique_ptr<std::vector<byte>> ram = std::unique_ptr<std::vector<byte>>(new std::vector<byte>(ramSize));
        this->prgRam = std::unique_ptr<MemoryRepeater>( new MemoryRepeater(0x6000, 0x7FFF, std::move(ram)) );
    }
    
    //no chrRom
    return true;
}