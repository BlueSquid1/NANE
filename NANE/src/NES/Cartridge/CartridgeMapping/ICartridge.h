#ifndef I_CARTRIDGE
#define I_CARTRIDGE

#include "NES/Util/BitUtil.h"
#include "NES/Memory/IMemoryRW.h"

class ICartridge : public IMemoryRW
{
    private:
    const unsigned int mapNumber = 0;
    std::unique_ptr<byte> prgRom = NULL;
    std::unique_ptr<byte> chrRom = NULL;

    public:
    //constructor
    ICartridge(int mapNum) : mapNumber(mapNum) {};
    //virtual deconstructor
    virtual ~ICartridge() {};
    
    virtual bool LoadINes(std::unique_ptr<INes> INesRom);
    virtual byte Read(dword address);
    virtual void Write(dword address, byte value);
};
#endif