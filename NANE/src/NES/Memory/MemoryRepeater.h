#ifndef MEMORY_REPEATER
#define MEMORY_REPEATER

#include "IMemoryRW.h"
#include "NES/Util/BitUtil.h"
#include <exception>
#include <cmath>

class MemoryRepeater : public IMemoryRW
{
    private:

    std::unique_ptr<std::vector<byte>> data = NULL;

    public:
    //constructor
    MemoryRepeater(dword startAddress, dword endAddress, std::unique_ptr<std::vector<byte>> data = NULL);

    dword LowerOffset(dword origionalAddress);

    byte Read(dword address);
    void Write(dword address, byte value);
};

#endif