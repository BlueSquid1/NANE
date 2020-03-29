#ifndef PPU_MEMORY_MAP
#define PPU_MEMORY_MAP

#include <memory>

#include "PpuRegisters.h"
#include "ColourPalettes.h"
#include "PatternTables.h"
#include "NES/Memory/BitUtil.h"
#include "NES/Memory/IMemoryRW.h"
#include "NES/Memory/MemoryRepeaterArray.h"
#include "NES/Cartridge/CartridgeMapping/ICartridge.h"
#include "NES/APU/ApuRegisters.h"


class PpuMemoryMap : public IMemoryRW
{
    private:
    std::shared_ptr<PpuRegisters> ppuRegMem = NULL;

    std::shared_ptr<ICartridge> cartridge = NULL; //pattern memory (chr rom)
    std::unique_ptr<IMemoryRepeater> nametableMem = NULL; //layout (vram)
    std::unique_ptr<ColourPalettes> palettesMem = NULL; //colours
    
    std::unique_ptr<std::vector<byte>> primOam = NULL; //256 bytes
    std::unique_ptr<std::vector<byte>> secOam = NULL; //32 bytes (8 sprites to render on scanline)

    int scanlineNum = -1; //the current scanline being rendered between -1 and 260
    int scanCycleNum = 0; //the cycle num for the current scanline between 0 and 340

    public:
    //constructor
    PpuMemoryMap(std::shared_ptr<PpuRegisters> ppuRegisters);
    virtual byte Read(dword address) const override;
    virtual void Write(dword address, byte value) override;

    std::unique_ptr<PatternTables> GeneratePatternTablesFromRom();

    //getters and setters
    void SetCartridge(std::shared_ptr<ICartridge> cartridge);
    std::unique_ptr<ColourPalettes>& GetPalettes();
    std::shared_ptr<PpuRegisters>& GetPpuRegisters();
    void SetScanLineNum(int scanLineNum);
    int GetScanLineNum() const;
    void SetScanCycleNum(int scanCycleNum);
    int GetScanCycleNum() const;
};
#endif