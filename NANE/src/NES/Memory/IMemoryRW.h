#ifndef I_MEMORY_RW
#define I_MEMORY_RW

#include "IMemoryR.h"

class IMemoryRW : public IMemoryR
{
    public:
    //constructor
    IMemoryRW(dword startAddress, dword endAddress);
    //virtual deconstructor
    virtual ~IMemoryRW();
    virtual void Write(dword address, byte value) = 0;
};

#endif