#ifndef DMA
#define DMA

#include "NES/CPU/CpuMemoryMap.h"
#include "NES/PPU/PpuMemoryMap.h"
#include "NES/APU/ApuRegisters.h"
#include "NES/Memory/IMemoryRW.h"
#include "NES/Cartridge/CartridgeMapping/ICartridge.h"

#include <memory> //std::unique_ptr


//keeps track of all memory used by NES console
//in other words its the memory bus
class Dma : public IMemoryRW
{
    CpuMemoryMap cpuMemory;
    PpuMemoryMap ppuMemory;
    ApuRegisters apuRegisters;
    std::unique_ptr<ICartridge> cartridge = NULL;

    public:
    Dma();
    
    byte CpuRead(dword address);
    void CpuWrite(dword address, byte value);

    byte PpuRead(dword address);
    void PpuWrite(dword address, byte value);

    //defaults to CPU read/write
    virtual byte Read(dword address) override;
    virtual void Write(dword address, byte value) override;

    std::unique_ptr<PatternTables> GeneratePatternTablesFromRom();

    //getters/setters
    CpuMemoryMap& GetCpuMemory();
    PpuMemoryMap& GetPpuMemory();
    ApuRegisters& GetApuRegisters();
    bool SetCartridge(std::unique_ptr<ICartridge> cartridge);
    std::unique_ptr<ICartridge>& GetCartridge();
};

#endif