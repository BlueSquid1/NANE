#include "ICartridge.h"

ICartridge::ICartridge(int mapNum)
    : IMemoryRW(0x0000, 0xFFFF),
      mapNumber(mapNum)
{
}

ICartridge::~ICartridge()
{

}

bool ICartridge::Contains(dword address) const
{
    bool result = false;
    if(this->prgRom != NULL && this->prgRom->Contains(address))
    {
        return true;
    }
    else if(this->chrRom != NULL && this->chrRom->Contains(address))
    {
        return true;
    }
    else if(this->prgRam != NULL && this->prgRam->Contains(address))
    {
        return true;
    }

    return false;
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

byte ICartridge::Seek(dword address) const
{
    if(this->prgRom->Contains(address))
    {
        return this->prgRom->Seek(address);
    }
    else if(this->chrRom != NULL && this->chrRom->Contains(address))
    {
        return this->chrRom->Seek(address);
    }
    else if(this->prgRam != NULL && this->prgRam->Contains(address))
    {
        return this->prgRam->Seek(address);
    }
    else
    {
        std::cerr << "cartridge: couldn't seek to memory address: " << address << std::endl;
    }
    return 0;
}

const unsigned int ICartridge::GetMapNumber() const
{
    return this->mapNumber;
}

std::shared_ptr<MemoryRepeaterVec> ICartridge::GetChrRom()
{
    return this->chrRom;
}

std::shared_ptr<MemoryRepeaterVec> ICartridge::GetPrgRom()
{
    return this->prgRom;
}

INes::MirrorType ICartridge::GetMirroringType()
{
    return this->mirroringType;
}

void ICartridge::SetMirroringType(INes::MirrorType mirroringType)
{
    this->mirroringType = mirroringType;
}