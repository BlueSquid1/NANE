#include "Cpu.h"

Cpu::Cpu(std::unique_ptr<CpuRegisters> cpuRegisters, std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters)
{
    this->registers = std::move(cpuRegisters);
    this->memory = std::unique_ptr<CpuMemoryMap>( new CpuMemoryMap(ppuRegisters, apuRegisters) );
}

void Cpu::Step()
{
    //get optCode from memory
    byte optCode = this->memory->Read(this->registers->name.PC);
    Instruction instruction = instructionSet.GetDetails(optCode);

    //get arguments

    //print instruction

    //set the address temp variable

    //execute instructions
    switch(instruction.instructionType)
    {
        
    }

    //return cycles
}