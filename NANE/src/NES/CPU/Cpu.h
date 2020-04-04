#ifndef CPU
#define CPU

#include "Instructions.h"
#include "NES/Memory/BitUtil.h"
#include "NES/Memory/Dma.h"

#include <string> //std::string

class Cpu
{
    private:
    Dma& dma;

    int totalClockCycles = 0;

    void UpdateRegsForOverflow(byte oldAValue, byte inputVal);
    void UpdateRegsForOverflowNeg(byte oldAValue, byte inputVal);
    void UpdateRegsForZeroAndNeg(byte inputVal);
    void UpdateRegsForCompaire(byte value, byte inputVal);
    void Push(dword value);
    void Push(byte value);
    byte Pop();
    int AdditionalCyclesForPageCross(dword address1, dword address2);
    std::unique_ptr<Instructions::Instruction> DecodeInstruction(dword pcAddress, bool seekOnly = false);
    CpuRegisters& GetRegs();

    public:
    Cpu(Dma& dma);
    bool PowerCycle(dword newPcAddress = 0x8000);
    int Step(bool verbose);

    /**
     * @param instructionsBefore how many instructions to print before current instructions
     * @param instructionsAfter how many instructions to print after current instructions
     */
    std::string GenerateCpuScreen(int instructionsBefore = 10, int instructionsAfter = 10);
    
    //getters and setters
    void SetTotalClockCycles(int totalCycles);
};

#endif