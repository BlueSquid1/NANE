#include "ICartridge.h"

ICartridge::ICartridge(int mapNum)
    : IMemoryRW(0x4020, 0xFFFF),
      mapNumber(mapNum)
{
}

ICartridge::~ICartridge()
{

}

byte ICartridge::Read(dword address)
{
    if(this->prgRom->Contains(address))
    {
        return this->prgRom->Read(address);
    }
    else if(this->chrRom != NULL && this->chrRom->Contains(address))
    {
        return this->chrRom->Read(address);
    }
    else if(this->prgRam != NULL && this->prgRam->Contains(address))
    {
        return this->prgRam->Read(address);
    }
    else
    {
        std::cerr << "cartridge: couldn't read at memory address: " << address << std::endl;
    }
    return 0;
}

void ICartridge::Write(dword address, byte value)
{
    if(this->prgRom->Contains(address))
    {
        this->prgRom->Write(address, value);
    }
    else if(this->chrRom->Contains(address))
    {
        this->chrRom->Write(address, value);
    }
    else if(this->prgRam->Contains(address))
    {
        this->prgRam->Write(address, value);
    }
    else
    {
        std::cerr << "cartridge: couldn't write at memory address: " << address << std::endl;
    }
}

const unsigned int ICartridge::GetMapNumber()
{
    return this->mapNumber;
}