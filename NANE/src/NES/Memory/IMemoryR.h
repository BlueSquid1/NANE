#ifndef I_MEMORY_R
#define I_MEMORY_R

#include "Types.h"

class IMemoryR
{
    public:
    //first valid address
    dword startAddress = 0;
    //last valid address
    dword endAddress = 0;

    //constructor
    IMemoryR(dword startAddress, dword endAddress);

    //virtual deconstructor
    virtual ~IMemoryR();

    virtual bool Contains(dword address) const;

    /**
     * @param address the address to loopup
     */
    virtual byte Read(dword address) = 0;
    
    virtual byte Seek(dword address) const = 0;
};

#endif