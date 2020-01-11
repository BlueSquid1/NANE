#ifndef I_MEMORY_R
#define I_MEMORY_R

#include "Types.h"

class IMemoryR
{
    protected:
    //first valid address
    dword startAddress = 0;
    //last valid address
    dword endAddress = 0;

    public:
    //constructor
    IMemoryR(dword startAddress, dword endAddress);

    //virtual deconstructor
    virtual ~IMemoryR();

    virtual bool Contains(dword address) const;

    virtual byte Read(dword address) const = 0;
};

#endif