#ifndef CPU
#define CPU

#include <memory> //std::unique_ptr
#include <iostream> //std::string
#include <sstream> //std::stringstream
#include <iomanip> //std::setfill and std::setw

#include "NES/Memory/BitUtil.h"
#include "CpuMemoryMap.h"
#include "Instructions.h"
#include "NES/PPU/Matrix.h"
#include "NES/PPU/NesColour.h"

class Cpu
{
    private:
    std::unique_ptr<CpuMemoryMap> cpuMemory = NULL;

    int totalClockCycles;

    void UpdateRegsForOverflow(byte oldAValue, byte inputVal);
    void UpdateRegsForOverflowNeg(byte oldAValue, byte inputVal);
    void UpdateRegsForZeroAndNeg(byte inputVal);
    void UpdateRegsForCompaire(byte value, byte inputVal);
    void Push(dword value);
    void Push(byte value);
    byte Pop();
    int AdditionalCyclesForPageCross(dword address1, dword address2);

    std::unique_ptr<Instructions::Instruction> DecodeInstruction(dword address);

    public:
    Cpu(std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters);
    bool PowerCycle(dword newPcAddress = 0x8000);
    int Step(bool verbose);
    bool SetCartridge(std::shared_ptr<ICartridge> cartridge);

    /**
     * @param instructionsBefore how many instructions to print before current instructions
     * @param instructionsAfter how many instructions to print after current instructions
     */
    std::string GenerateCpuScreen(int instructionsBefore = 10, int instructionsAfter = 10);
    
    //getters and setters
    CpuRegisters * GetRegisters();
    void SetTotalClockCycles(int totalCycles);
};

#endif