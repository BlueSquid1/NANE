#pragma once

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

    virtual byte Read(dword address) override;
    virtual void Write(dword address, byte value) override;
    virtual byte Seek(dword address) const override;
};
