#ifndef INSTRUCTION_SET
#define INSTRUCTION_SET

#include "NES/Memory/BitUtil.h"

enum InstructionType
{

};

enum AddressMode
{
    
};

struct Instruction
{
    byte optCode;
    unsigned int optLength;
    InstructionType instructionType;
    AddressMode addressMode;
    unsigned int baseCycles;
};

/**
 * responsible for decoding instructions
 * http://obelisk.me.uk/6502/reference.html
 */
class InstructionSet
{
    //hashtable<byte, Instruction>
    public:
    Instruction GetDetails(byte optCode);
};

#endif