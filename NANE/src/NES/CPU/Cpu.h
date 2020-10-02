#ifndef CPU
#define CPU

#include "Instructions.h"
#include "NES/Memory/BitUtil.h"
#include "NES/Memory/Dma.h"

#include <string> //std::string

class Cpu
{
    private:
    enum class InterruptType
    {
        breakCommand,
        nmiCommand,
        irqCommand
    };

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
    std::unique_ptr<Instructions::Instruction> DecodeInstruction(dword pcAddress, bool seekOnly = false, bool verbose = false);
    CpuRegisters& GetRegs();
    byte SeekOrRead(dword address, bool seekOnly);

    public:
    Cpu(Dma& dma);
    bool PowerCycle(dword * overridePcAddress = NULL);
    int Step(bool verbose);
    int HandleIrqEvent(bool verbose);
    int HandleNmiEvent(bool verbose);
    void HandleInterrupt(InterruptType interruptType, bool verbose);

    /**
     * @param instructionsBefore how many instructions to print before current instructions
     * @param instructionsAfter how many instructions to print after current instructions
     */
    std::string GenerateCpuScreen(int instructionsBefore, int instructionsAfter);
    
    //getters and setters
    void SetTotalClockCycles(int totalCycles);
};

#endif