#ifndef DMA
#define DMA

#include "NES/CPU/CpuMemoryMap.h"
#include "NES/PPU/PpuMemoryMap.h"
#include "NES/APU/ApuRegisters.h"
#include "NES/Cartridge/CartridgeMapping/ICartridge.h"

#include <memory> //std::unique_ptr

//keeps track of all memory used by NES console
//in other words its the memory bus
class Dma : public IMemoryRW
{
    private:
    CpuMemoryMap cpuMemory;
    PpuMemoryMap ppuMemory;
    ApuRegisters apuRegisters;
    std::unique_ptr<ICartridge> cartridge = NULL;
    bool nmiActive = false;

    void IncrementPpuAddress();

    public:
    Dma();
    
    byte Read(dword address) override;
    void Write(dword address, byte value) override;
    byte Seek(dword address) const override;

    byte PpuRead(dword address);
    void PpuWrite(dword address, byte value);
    
    bool SetCartridge(std::unique_ptr<ICartridge> cartridge);

    std::unique_ptr<PatternTables> GeneratePatternTablesFromRom();

    //getters/setters
    CpuMemoryMap& GetCpuMemory();
    PpuMemoryMap& GetPpuMemory();
    ApuRegisters& GetApuRegisters();
    std::unique_ptr<ICartridge>& GetCartridge();
    bool GetNmi();
    void SetNmi(bool isActive);
};

#endif