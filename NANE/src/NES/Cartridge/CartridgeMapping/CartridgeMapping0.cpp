#include "CartridgeMapping0.h"

CartridgeMapping0::CartridgeMapping0()
: ICartridge(0)
{

}

bool CartridgeMapping0::LoadINes(std::unique_ptr<INes> INesRom)
{
    this->mirroringType = INesRom->GetMirroringType();
    this->prgRom = std::unique_ptr<MemoryRepeaterVec>( new MemoryRepeaterVec(0x8000, 0xFFFF, INesRom->GetPrgRomData()) );
    this->chrRom = std::unique_ptr<MemoryRepeaterVec>( new MemoryRepeaterVec(0x0000, 0x1FFF, INesRom->GetChrRomData()) );

    if(INesRom->GetBatteryPresent())
    {
        dword ramSize = 8192;
        std::unique_ptr<std::vector<byte>> ram = std::unique_ptr<std::vector<byte>>(new std::vector<byte>(ramSize));
        this->prgRam = std::unique_ptr<MemoryRepeaterVec>( new MemoryRepeaterVec(0x6000, 0x7FFF, std::move(ram)) );
    }
    return true;
}