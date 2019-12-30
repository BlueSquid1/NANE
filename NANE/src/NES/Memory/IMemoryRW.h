#ifndef I_MEMORY_RW
#define I_MEMORY_RW

#include "IMemoryR.h"

class IMemoryRW : public IMemoryR
{
    public:
    //virtual deconstructor
    virtual ~IMemoryRW() {};
    virtual void Write(dword address, byte value);
};

#endif