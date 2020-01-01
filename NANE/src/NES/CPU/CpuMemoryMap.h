#ifndef CPU_MEMORY_MAP
#define CPU_MEMORY_MAP

#include <memory> //std::shared_ptr

#include "CpuRegisters.h"
#include "NES/Memory/BitUtil.h"
#include "NES/PPU/PpuRegisters.h"
#include "NES/APU/ApuRegisters.h"
#include "NES/Cartridge/CartridgeMapping/ICartridge.h"
#include "NES/Memory/IMemoryRW.h"
#include "NES/Memory/MemoryRepeater.h"


class CpuMemoryMap : public IMemoryRW
{
    private:
    std::unique_ptr<MemoryRepeater> cpuRam = NULL;
    
    std::shared_ptr<PpuRegisters> ppuRegisters = NULL;
    std::shared_ptr<MemoryRepeater> ppuRegMem = NULL;

    std::shared_ptr<ApuRegisters> apuRegisters = NULL;
    std::shared_ptr<MemoryRepeater> apuRegMem = NULL;

    std::shared_ptr<ICartridge> cartridge = NULL;

    public:
    CpuMemoryMap(std::shared_ptr<PpuRegisters> ppuRegisters, 
        std::shared_ptr<ApuRegisters> apuRegisters);

    virtual byte Read(dword address) const;
    virtual void Write(dword address, byte value);

    //getters and setters
    void SetCartridge(std::shared_ptr<ICartridge> cartridge);
};
#endif