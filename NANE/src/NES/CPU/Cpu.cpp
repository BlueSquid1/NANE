#include "Cpu.h"

Cpu::Cpu(std::unique_ptr<CpuRegisters> cpuRegisters, std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters)
{
    this->registers = std::move(cpuRegisters);
    this->memory = std::unique_ptr<CpuMemoryMap>( new CpuMemoryMap(ppuRegisters, apuRegisters) );
}

bool Cpu::PowerCycle()
{
    //TODO
    this->registers->name.PC = 0xC000;
    return true;
}

void Cpu::Step()
{
    //get optCode from memory
    byte optCode = this->memory->Read(this->registers->name.PC);

    std::cout << int(optCode) << std::endl;
    Instruction instruction = instructionSet.GetDetails(optCode);
    union
    {
        byte byteVal;
        dword dwordVal;
    } optArg;

    //get arguments
    if(instruction.optLength == 2)
    {
        optArg.byteVal = this->memory->Read(this->registers->name.PC + 1);
    }
    else if(instruction.optLength == 3)
    {
        optArg.dwordVal = BitUtil::GetDWord(this->memory.get(), this->registers->name.PC + 1);
    }

    //print instruction

    //set the address temp variable

    //execute instructions
    //switch(instruction.instructionType)
    {
        
    }

    //return cycles
}


bool Cpu::SetCartridge(std::shared_ptr<ICartridge> cartridge)
{
    this->memory->SetCartridge(cartridge);
    return true;
}