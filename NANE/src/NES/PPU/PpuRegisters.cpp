#include "PpuRegisters.h"

const int PpuRegisters::rawLen;

PpuRegisters::PpuRegisters()
{
    memset(this->raw, 0, this->rawLen);
}