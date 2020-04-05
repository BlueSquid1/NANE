#include "NameTables.h"

const int NameTables::rawLen;

NameTables::NameTables()
 : MemoryRepeaterArray(0x2000, 0x3EFF, this->raw, this->rawLen)
{
    memset(this->raw, 0, this->rawLen);
}

dword NameTables::Redirect(dword address) const
{
    dword redirrectedAddress = address;
    dword offset = this->LowerOffset(address);
    switch(this->mirroringType)
    {
        case INes::vertical:
        {
            if(offset >= 0x0800)
            {
                redirrectedAddress -= 0x0800;
            }
            break;
        }
        case INes::horizontal:
        {
            if((offset >= 0x0400 && offset < 0x0800) || offset >= 0x0C00)
            {
                redirrectedAddress -= 0x0400;
            }
            break;
        }
        default:
        {
            std::cerr << "Haven't implemented the name tabling mapping" << std::endl;
        }
    }
    return redirrectedAddress;
}


byte NameTables::Read(dword address)
{
    if(this->mirroringType == INes::not_set)
    {
        std::cerr << "warning, mirroring type for PPU Name tables hasn't been set. Expect corrupt graphics." << std::endl;
    }
    return this->Seek(address);
}

void NameTables::Write(dword address, byte value)
{
    if(this->mirroringType == INes::not_set)
    {
        std::cerr << "warning, mirroring type for PPU Name tables hasn't been set. Expect corrupt graphics." << std::endl;
    }
    dword redirrectAddress = this->Redirect(address);
    return MemoryRepeaterArray::Write(redirrectAddress, value);
}

byte NameTables::Seek(dword address) const
{
    if(this->mirroringType == INes::not_set)
    {
        std::cerr << "warning, mirroring type for PPU Name tables hasn't been set. Expect corrupt graphics." << std::endl;
    }
    dword redirrectAddress = this->Redirect(address);
    return MemoryRepeaterArray::Seek(redirrectAddress);
}

void NameTables::SetMirrorType( INes::MirrorType mirroringType )
{
    this->mirroringType = mirroringType;
}