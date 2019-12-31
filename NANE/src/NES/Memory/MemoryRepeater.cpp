#include "MemoryRepeater.h"

MemoryRepeater::MemoryRepeater(dword startAddress, dword endAddress, std::unique_ptr<std::vector<byte>> data)
    :IMemoryRW(startAddress, endAddress)
{
    this->startAddress = startAddress;
    this->endAddress = endAddress;
    this->data = std::move(data);
}

dword MemoryRepeater::LowerOffset(dword origionalAddress)
{
    dword offset = origionalAddress - this->startAddress;
    dword lowerOffset = remainder(offset, this->data->size());
    return lowerOffset;
}
#include <iostream>
byte MemoryRepeater::Read(dword address)
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
    return this->data->at(lowerOffset);
}

void MemoryRepeater::Write(dword address, byte value)
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
    this->data->at(lowerOffset) = value;
}