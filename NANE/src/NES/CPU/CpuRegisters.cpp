#include "CpuRegisters.h"

CpuRegisters::CpuRegisters()
{
    memset(this->raw, 0, this->rawLen);
}