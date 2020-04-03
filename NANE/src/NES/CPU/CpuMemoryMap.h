#ifndef CPU_MEMORY_MAP
#define CPU_MEMORY_MAP

#include <memory> //std::unique_ptr

#include "CpuRegisters.h"
#include "NES/Memory/BitUtil.h"
#include "NES/Memory/IMemoryRW.h"
#include "NES/Memory/MemoryRepeaterVec.h"


class CpuMemoryMap : public IMemoryRW
{
    private:
    CpuRegisters cpuRegMem;
    std::unique_ptr<MemoryRepeaterVec> cpuRam = NULL;

    public:
    CpuMemoryMap();

    virtual byte Read(dword address) override;
    virtual void Write(dword address, byte value) override;

    //getters and setters
    CpuRegisters& GetRegisters();
};
#endif