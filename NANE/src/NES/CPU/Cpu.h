#ifndef CPU
#define CPU

#include <memory> //std::unique_ptr

#include "NES/Util/BitUtil.h"
#include "CpuMemoryMap.h"
#include "CpuRegisters.h"

class Cpu
{
    private:
    std::unique_ptr<CpuRegisters> r;
    std::unique_ptr<CpuMemoryMap> m;

    public:
    Cpu(std::unique_ptr<CpuMemoryMap> cpuMemoryMap);
    void PowerCycle();
    void ExecuteInstruction();
};

#endif