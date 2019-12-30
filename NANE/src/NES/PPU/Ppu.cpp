#include "Ppu.h"

Ppu::Ppu(std::shared_ptr<PpuRegisters> ppuRegisters)
{
    this->registers = ppuRegisters;
    this->memory = std::unique_ptr<PpuMemoryMap>();
}