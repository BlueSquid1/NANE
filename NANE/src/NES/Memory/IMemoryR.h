#ifndef I_MEMORY_R
#define I_MEMORY_R

#include "NES/Util/BitUtil.h"

class IMemoryR
{
    public:
    //virtual deconstructor
    virtual ~IMemoryR() {};
    virtual byte Read(dword address) = 0;
};

#endif