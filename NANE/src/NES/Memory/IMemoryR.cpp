#include "IMemoryR.h"

IMemoryR::IMemoryR(dword startAddress, dword endAddress)
{
    this->startAddress = startAddress;
    this->endAddress = endAddress;
}

bool IMemoryR::Contains(dword address)
{
    return (address >= this->startAddress) && (address <= this->endAddress);
}

IMemoryR::~IMemoryR()
{

}