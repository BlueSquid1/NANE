#include "CpuRegisters.h"

const int CpuRegisters::rawLen;

CpuRegisters::CpuRegisters()
{
    memset(this->raw, 0, this->rawLen);
}