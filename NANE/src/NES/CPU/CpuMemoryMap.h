#ifndef CPU_MEMORY_MAP
#define CPU_MEMORY_MAP

#include <memory> //std::shared_ptr
#include <vector> //std::vector

#include "CpuRegisters.h"
#include "NES/Util/BitUtil.h"
#include "NES/PPU/PpuRegisters.h"
#include "NES/APU/ApuRegisters.h"
#include "NES/Cartridge/CartridgeMapping/ICartridge.h"
#include "NES/Memory/IMemoryRW.h"

class CpuMemoryMap : public IMemoryRW
{
    private:
    std::vector<byte> cpuRam; //2048 bytes
    std::shared_ptr<PpuRegisters> ppuRegisters = NULL;
    std::shared_ptr<ApuRegisters> apuRegisters = NULL;
    std::shared_ptr<ICartridge> cartridge = NULL;

    public:
    CpuMemoryMap(std::shared_ptr<PpuRegisters> ppuRegisters, 
        std::shared_ptr<ApuRegisters> apuRegisters);

    virtual byte Read(dword address);
    virtual void Write(dword address, byte value);

    //getters and setters
    void SetCartridge(std::shared_ptr<ICartridge> cartridge);
};
#endif