#pragma once

#include "IMemoryRepeater.h"
#include "NES/Memory/BitUtil.h"
#include <exception>
#include <cmath>

class MemoryRepeaterVec : public IMemoryRepeater
{
    private:
    std::shared_ptr<std::vector<byte>> data = NULL;

    public:
    //constructors
    MemoryRepeaterVec(dword startAddress, dword endAddress, std::shared_ptr<std::vector<byte>> data);

    virtual byte Read(dword address) override;
    virtual void Write(dword address, byte value) override;
    virtual byte Seek(dword address) const override;

    //getters and setters
    std::shared_ptr<std::vector<byte>> GetDataVec(); 
};