#ifndef PPU_MEMORY_MAP
#define PPU_MEMORY_MAP

#include "PpuRegisters.h"
#include "ColourPalettes.h"
#include "PatternTables.h"
#include "NameTables.h"
#include "OamPrimary.h"
#include "OamSecondary.h"
#include "NES/Memory/BitUtil.h"
#include "NES/Memory/IMemoryRW.h"

class PpuMemoryMap : public IMemoryRW
{
    private:
    PpuRegisters ppuRegMem;
    NameTables nametableMem; //layout (vram)
    ColourPalettes palettesMem; //colours
    OamPrimary primaryOamMem; //all active sprites
    OamSecondary secondaryOamMem; //just active sprites on current scanline
    
    //std::vector<byte> secOam; //32 bytes (8 sprites to render on scanline)

    //these are here because this state is useful when restoring the PPU to a previous save
    int scanlineNum = -1; //the current scanline being rendered between -1 and 260
    int scanCycleNum = 0; //the cycle num for the current scanline between 0 and 340
    long long totalPpuCycles = 0;

    public:
    //constructor
    PpuMemoryMap();
    virtual byte Read(dword address) override;
    virtual void Write(dword address, byte value) override;
    byte Seek(dword address) const override;
    dword GetVRamAddress(bool forWriting) const;

    //getters and setters
    ColourPalettes& GetPalettes();
    PpuRegisters& GetRegisters();
    const PpuRegisters& GetRegisters() const;
    NameTables& GetNameTables();
    OamPrimary& GetPrimaryOam();
    OamSecondary& GetSecondaryOam();
    void SetScanLineNum(int scanLineNum);
    int GetScanLineNum() const;
    void SetScanCycleNum(int scanCycleNum);
    int GetScanCycleNum() const;
    long long& GetTotalPpuCycles();
    void SetTotalPpuCycles(long long& cycles);
};
#endif