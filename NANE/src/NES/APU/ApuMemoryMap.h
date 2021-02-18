#pragma once

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/IMemoryRW.h"

#include "ApuRegisters.h"

class ApuMemoryMap : public IMemoryRW
{
    private:
    ApuRegisters apuRegMem;


    long long totalApuCycles = 0;

    public:
    //constructor
    ApuMemoryMap();
    byte Read(dword address) override;
    void Write(dword address, byte value) override;
    byte Seek(dword address) const override;
    bool Contains(dword address) const override;

    //getters and setters
    const long long& GetTotalApuCycles() const;
    void SetTotalApuCycles(const long long& value);
    ApuRegisters& GetApuRegisters();
};