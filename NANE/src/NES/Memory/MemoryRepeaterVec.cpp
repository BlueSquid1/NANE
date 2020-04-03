#include "MemoryRepeaterVec.h"

MemoryRepeaterVec::MemoryRepeaterVec(dword startAddress, dword endAddress, std::unique_ptr<std::vector<byte>> data)
    : IMemoryRepeater(startAddress, endAddress, data->size())
{
    this->data = std::move(data);
}

byte MemoryRepeaterVec::Read(dword address)
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

void MemoryRepeaterVec::Write(dword address, byte value)
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

std::unique_ptr<std::vector<byte>>& MemoryRepeaterVec::GetDataVec()
{
    return this->data;
}