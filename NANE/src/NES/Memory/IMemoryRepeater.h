#ifndef IMEMORY_REPEATER
#define IMEMORY_REPEATER

#include "IMemoryRW.h"
#include "NES/Memory/BitUtil.h"
#include <exception>
#include <cmath>

class IMemoryRepeater : public IMemoryRW
{
    public:
    dword dataLen = 0;

    IMemoryRepeater(dword startAddress, dword endAddress, dword dataLen);
    dword LowerOffset(dword origionalAddress) const;
};

#endif