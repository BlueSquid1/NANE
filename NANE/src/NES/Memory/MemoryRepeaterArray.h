#ifndef MEMORY_REPEATER_ARRAY
#define MEMORY_REPEATER_ARRAY

#include "IMemoryRepeater.h"
#include "NES/Memory/BitUtil.h"
#include <exception>
#include <cmath>

class MemoryRepeaterArray : public IMemoryRepeater
{
    private:
    byte * data = NULL;

    public:
    //constructors
    MemoryRepeaterArray(dword startAddress, dword endAddress, byte * data, dword length);

    virtual byte Read(dword address) const override;
    virtual void Write(dword address, byte value) override;
};

#endif