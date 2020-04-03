#include "MemoryRepeaterArray.h"

MemoryRepeaterArray::MemoryRepeaterArray(dword startAddress, dword endAddress, byte * data, dword length)
    : IMemoryRepeater(startAddress, endAddress, length)
{
    this->data = data;
}

byte MemoryRepeaterArray::Read(dword address)
{
    //check boundaries
    if(!this->Contains(address))
    {
        throw std::invalid_argument("try to read outside the boundary for a memory repeater");
    }
    else if (this->data == NULL)
    {
        throw std::invalid_argument("try to read when the data field hasn't been set");
    }
    dword lowerOffset = this->LowerOffset(address);
    if(lowerOffset < 0 || lowerOffset >= this->dataLen)
    {
        throw std::range_error("try to read outside the boundary of the array");
    }
    return this->data[lowerOffset];
}

void MemoryRepeaterArray::Write(dword address, byte value)
{
    //check boundaries
    if(!this->Contains(address))
    {
        throw std::invalid_argument("try to write outside the boundary for a memory repeater");
    }
    else if (this->data == NULL)
    {
        throw std::invalid_argument("try to write when the data field hasn't been set");
    }
    dword lowerOffset = this->LowerOffset(address);
    this->data[lowerOffset] = value;
}