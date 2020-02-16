#include "IMemoryRepeater.h"

IMemoryRepeater::IMemoryRepeater(dword startAddress, dword endAddress, dword dataLen)
: IMemoryRW(startAddress, endAddress)
{
    this->dataLen = dataLen;
}

dword IMemoryRepeater::LowerOffset(dword origionalAddress) const
{
    dword dataLen = this->dataLen;
    dword offset = origionalAddress - this->startAddress;
    dword lowerOffset = fmod(offset, dataLen);
    return lowerOffset;
}