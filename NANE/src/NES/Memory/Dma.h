#pragma once

#include "NES/CPU/CpuMemoryMap.h"
#include "NES/PPU/PpuMemoryMap.h"
#include "NES/APU/ApuMemoryMap.h"
#include "NES/Cartridge/CartridgeMapping/ICartridge.h"
#include "NES/Controller/ControllerManager.h"

#include <memory> //std::unique_ptr

//keeps track of all memory used by NES console
//in other words its the memory bus
class Dma : public IMemoryRW
{
    public:
    enum DmaAddresses : unsigned
    {
        DMA_ADDR   = 0x4014
    };

    private:
    CpuMemoryMap cpuMemory;
    PpuMemoryMap ppuMemory;
    ApuMemoryMap apuMemory;
    ControllerManager ControllerMgr;
    std::shared_ptr<ICartridge> cartridge = NULL;
    bool nmiActive = false;

    bool dmaActive = false;
    bool dmaGoodCycle = false;
    dword dmaBaseAddress = 0;
    dword dmaAddressOffset = 0;
    byte dmaBuffer = 0;

    public:
    Dma();
    
    byte Read(dword address) override;
    void Write(dword address, byte value) override;
    byte Seek(dword address) const override;

    void ProcessDma();

    bool IsDmaActive();

    byte PpuRead(dword address);
    void PpuWrite(dword address, byte value);
    
    bool SetCartridge(std::shared_ptr<ICartridge> cartridge);
    PatternTables::BitTile& GetPatternTile(int tableNum, patternIndex patternNum);

    //for disassembly
    std::unique_ptr<PatternTables> GeneratePatternTablesFromRom();

    //getters/setters
    const std::shared_ptr<ICartridge>& GetCartridge() const;
    CpuMemoryMap& GetCpuMemory();
    PpuMemoryMap& GetPpuMemory();
    ApuMemoryMap& GetApuMemory();
    ControllerManager& GetControllerManager();
    std::shared_ptr<ICartridge>& GetCartridge();
    bool GetNmi();
    void SetNmi(bool isActive);
    bool GetDmaActive() const;
    bool GetDmaGoodCycle() const;
    void SetDmaGoodCycle(bool value);
    dword GetDmaBaseAddress();
    dword GetDmaAddressOffset();
    void SetDmaAddressOffset(dword address);
    byte GetDmaBuffer();
    void SetDmaBuffer(byte value);
};
