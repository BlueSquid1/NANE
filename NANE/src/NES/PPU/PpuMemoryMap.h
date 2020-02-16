#ifndef PPU_MEMORY_MAP
#define PPU_MEMORY_MAP

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/IMemoryRW.h"
#include "NES/Cartridge/CartridgeMapping/ICartridge.h"
#include "NES/APU/ApuRegisters.h"
#include "ColourPalettes.h"

class PpuMemoryMap : public IMemoryRW
{
    private:
    std::unique_ptr<MemoryRepeaterVec> vram = NULL;
    std::shared_ptr<ColourPalettes> ppuPalettes = NULL;
    std::shared_ptr<ICartridge> cartridge = NULL;

    public:
    //constructor
    PpuMemoryMap(std::shared_ptr<ColourPalettes> ppuPalettes);
    virtual byte Read(dword address) const override;
    virtual void Write(dword address, byte value) override;

    //getters and setters
    void SetCartridge(std::shared_ptr<ICartridge> cartridge);
};
#endif