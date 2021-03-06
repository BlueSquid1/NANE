#pragma once

#include <memory>

#include "../INes.h"
#include "NES/Memory/BitUtil.h"
#include "NES/Memory/IMemoryRW.h"
#include "NES/Memory/MemoryRepeaterVec.h"

class ICartridge : public IMemoryRW
{
    protected:
    const unsigned int mapNumber = 0;
    INes::MirrorType mirroringType = INes::MirrorType::not_set;

    std::shared_ptr<MemoryRepeaterVec> prgRom = NULL;
    std::shared_ptr<MemoryRepeaterVec> chrRom = NULL;
    std::shared_ptr<MemoryRepeaterVec> prgRam = NULL;

    //protected constructor to prevent constructing this interface
    ICartridge(int mapNum);
    
    public:
    //virtual deconstructor
    virtual ~ICartridge();

    virtual bool LoadINes(std::shared_ptr<INes> INesRom) = 0;

    bool Contains(dword address) const final;
    byte Read(dword address) override;
    void Write(dword address, byte value) override;
    byte Seek(dword address) const override;

    //getters and setters
    unsigned int GetMapNumber() const;
    std::shared_ptr<MemoryRepeaterVec> GetChrRom();
    std::shared_ptr<MemoryRepeaterVec> GetPrgRom();
    INes::MirrorType GetMirroringType();
    void SetMirroringType(INes::MirrorType mirroringType);
};