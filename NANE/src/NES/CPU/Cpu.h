#ifndef CPU
#define CPU

#include <memory> //std::unique_ptr

#include "NES/Memory/BitUtil.h"
#include "CpuMemoryMap.h"
#include "CpuRegisters.h"
#include "Instructions.h"

class Cpu
{
    private:
    std::unique_ptr<CpuRegisters> registers = NULL;
    std::unique_ptr<CpuMemoryMap> memory = NULL;

    void UpdateRegsForOverflow(byte inputVal);
    void UpdateRegsForZeroAndNeg(byte inputVal);
    void UpdateRegsForAccZeroAndNeg(byte inputVal);
    void UpdateRegsForCompaire(byte value);
    void Push(dword value);
    void Push(byte value);
    byte Pop();

    public:
    Cpu(std::unique_ptr<CpuRegisters> cpuRegisters, std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters);
    bool PowerCycle();
    int Step();
    bool SetCartridge(std::shared_ptr<ICartridge> cartridge);
};

#endif