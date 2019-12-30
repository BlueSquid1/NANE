#ifndef I_CARTRIDGE
#define I_CARTRIDGE

#include "NES/Util/BitUtil.h"

class ICartridge
{
    private:
    const unsigned int mapNumber = 0;
    std::unique_ptr<byte> prgRom = NULL;
    std::unique_ptr<byte> chrRom = NULL;

    public:
    byte Read(dword address);
    void Write(dword address, byte value);
};
#endif