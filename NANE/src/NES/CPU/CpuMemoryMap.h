#pragma once

#include <memory> //std::unique_ptr

#include "CpuRegisters.h"
#include "NES/Memory/BitUtil.h"
#include "NES/Memory/IMemoryRW.h"
#include "NES/Memory/MemoryRepeaterVec.h"

/**
 * handles the cpu address space
 */
class CpuMemoryMap : public IMemoryRW
{
    private:
    CpuRegisters cpuRegMem;
    std::unique_ptr<MemoryRepeaterVec> cpuRam = NULL;

    public:
    CpuMemoryMap();

    byte Read(dword address) override;
    void Write(dword address, byte value) override;
    byte Seek(dword address) const override;

    //getters and setters
    CpuRegisters& GetRegisters();
};