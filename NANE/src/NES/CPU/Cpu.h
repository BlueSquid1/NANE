#ifndef CPU
#define CPU

#include <memory> //std::unique_ptr
#include <iostream> //std::string
#include <sstream> //std::stringstream
#include <iomanip> //std::setfill and std::setw

#include "NES/Memory/BitUtil.h"
#include "CpuMemoryMap.h"
#include "CpuRegisters.h"
#include "Instructions.h"

class Cpu
{
    private:
    std::unique_ptr<CpuRegisters> registers = NULL;
    std::unique_ptr<CpuMemoryMap> memory = NULL;

    int totalClockCycles;

    void UpdateRegsForOverflow(byte oldAValue, byte inputVal);
    void UpdateRegsForOverflowNeg(byte oldAValue, byte inputVal);
    void UpdateRegsForZeroAndNeg(byte inputVal);
    void UpdateRegsForCompaire(byte value, byte inputVal);
    void Push(dword value);
    void Push(byte value);
    byte Pop();
    int AdditionalCyclesForPageCross(dword address1, dword address2);

    public:
    Cpu(std::unique_ptr<CpuRegisters> cpuRegisters, std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters);
    bool PowerCycle();
    int Step();
    bool SetCartridge(std::shared_ptr<ICartridge> cartridge);
    
    //getters and setters
    CpuRegisters * GetRegisters();

    void SetTotalClockCycles(int totalCycles);
};

#endif