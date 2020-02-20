#include "ColourPalettes.h"

ColourPalettes::ColourPalettes()
: MemoryRepeaterArray(0x3F00, 0x3F1F, this->raw, this->rawLen)
{
    memset(this->raw, 0, this->rawLen);
}

dword ColourPalettes::Redirect(dword address) const
{
    dword redirrectAddress = address;
    switch(address)
    {
        case 0x3F10:
            redirrectAddress = 0x3F00;
            break;
        case 0x3F14:
            redirrectAddress = 0x3F04;
            break;
        case 0x3F18:
            redirrectAddress = 0x3F08;
            break;
        case 0x3F1C:
            redirrectAddress = 0x33F0C;
            break;
    }
    return redirrectAddress;
}

byte ColourPalettes::Read(dword address) const
{
    dword redirrectAddress = this->Redirect(address);
    return MemoryRepeaterArray::Read(redirrectAddress);
}

void ColourPalettes::Write(dword address, byte value)
{
    dword redirrectAddress = this->Redirect(address);
    return MemoryRepeaterArray::Write(redirrectAddress, value);
}