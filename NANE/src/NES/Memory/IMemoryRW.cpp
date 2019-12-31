#include "IMemoryRW.h"

IMemoryRW::IMemoryRW(dword startAddress, dword endAddress)
    : IMemoryR(startAddress, endAddress)
{

}

IMemoryRW::~IMemoryRW()
{
    
}