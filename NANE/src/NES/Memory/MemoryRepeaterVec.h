#ifndef MEMORY_REPEATER_VEC
#define MEMORY_REPEATER_VEC

#include "IMemoryRepeater.h"
#include "NES/Memory/BitUtil.h"
#include <exception>
#include <cmath>

class MemoryRepeaterVec : public IMemoryRepeater
{
    private:
    std::unique_ptr<std::vector<byte>> data = NULL;

    public:
    //constructors
    MemoryRepeaterVec(dword startAddress, dword endAddress, std::unique_ptr<std::vector<byte>> data);

    virtual byte Read(dword address) const override;
    virtual void Write(dword address, byte value) override;
};

#endif