#include "Cpu.h"

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