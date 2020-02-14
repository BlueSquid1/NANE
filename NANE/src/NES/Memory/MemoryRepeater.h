#ifndef MEMORY_REPEATER
#define MEMORY_REPEATER

#include "IMemoryRW.h"
#include "NES/Memory/BitUtil.h"
#include <exception>
#include <cmath>

class MemoryRepeater : public IMemoryRW
{
    private:
    dword dataLen = 0;
    std::unique_ptr<std::vector<byte>> data = NULL;

    public:
    //constructors
    //TODO make it work with pointers instead of vectors
    MemoryRepeater(dword startAddress, dword endAddress, dword dataLen);
    MemoryRepeater(dword startAddress, dword endAddress, std::unique_ptr<std::vector<byte>> data);

    dword LowerOffset(dword origionalAddress) const;

    virtual byte Read(dword address) const override;
    virtual void Write(dword address, byte value) override;
};

#endif