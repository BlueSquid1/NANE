#ifndef CPU
#define CPU

#include <memory> //std::unique_ptr

#include "NES/Util/BitUtil.h"
#include "CpuMemoryMap.h"
#include "CpuRegisters.h"
#include "InstructionSet.h"

class Cpu
{
    private:
    std::unique_ptr<CpuRegisters> registers = NULL;
    std::unique_ptr<CpuMemoryMap> memory = NULL;

    InstructionSet instructionSet;


    public:
    Cpu(std::unique_ptr<CpuRegisters> cpuRegisters, std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters);
    void PowerCycle();
    void Step();
};

#endif