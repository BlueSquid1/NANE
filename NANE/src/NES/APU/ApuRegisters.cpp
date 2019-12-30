#include "ApuRegisters.h"

ApuRegisters::ApuRegisters()
{
    memset(this->raw, 0, this->rawLen);
}