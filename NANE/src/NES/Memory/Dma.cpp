#include "Dma.h"

#include <iostream> //std::cerr
#include <exception> //std::out_of_range
#include "NES/PPU/PatternTables.h"


Dma::Dma()
: IMemoryRW(0x0000, 0xFFFF)
{
}

byte Dma::Read(dword address)
{
    //handle special cases
    switch(address)
    {
        case PpuRegisters::PPUDATA_ADDR:
        {
            //read value to VRAM address
            byte returnVal = this->ppuMemory.GetRegisters().vRegs.ppuDataReadBuffer;
            dword vramAddress = this->ppuMemory.GetVRamAddress(false);
            this->ppuMemory.GetRegisters().vRegs.ppuDataReadBuffer = this->PpuRead(vramAddress);

            if(vramAddress >= 0x3F00)
            {
                //buffer is not delayed when accessing from 0x3F00 and above
                returnVal = this->ppuMemory.GetRegisters().vRegs.ppuDataReadBuffer;
            }
            this->ppuMemory.GetRegisters().IncrementVRamAddress();
            return returnVal;
            break;
        }
        case ApuRegisters::ApuAddresses::SND_CHN_ADDR:
        {
            ApuRegisters::RegStruct::ChannelStatus value;
            value.pulse1 = this->apuMemory.GetSquareWave1().GetWatchdogTimer() > 0;
            value.pulse2 = this->apuMemory.GetSquareWave2().GetWatchdogTimer() > 0;
            ///TODO
            
            value.frame_irq = this->irqActive;
            
            // Not sure if this is right but I noticed other emulators are doing it. Maybe it is to do with:
            // https://wiki.nesdev.com/w/index.php/APU#Status_.28.244015.29
            this->apuMemory.GetRegisters().name.irqInhibit = true;
            return value.SND_CHN;
            break;
        }
    }
    if(this->ControllerMgr.Contains(address))
    {
        return this->ControllerMgr.Read(address);
    }
    else if(this->ppuMemory.GetRegisters().Contains(address))
    {
        return this->ppuMemory.GetRegisters().Read(address);
    }
    else if(this->apuMemory.Contains(address))
    {
        return this->apuMemory.Read(address);
    }
    else if(this->cpuMemory.Contains(address))
    {
        return this->cpuMemory.Read(address);
    }
    else if(this->cartridge != NULL &&this->cartridge->Contains(address))
    {
        return this->cartridge->Read(address);
    }
    else
    {
        throw std::out_of_range("DMA: tried to CPU read from an invalid memory address.");
    }
}

void Dma::Write(dword address, byte value)
{
    //handle special cases
    switch(address)
    {
        case PpuRegisters::OAMDATA_ADDR:
        {
            this->ppuMemory.GetPrimaryOam().Write(this->ppuMemory.GetRegisters().name.OAMADDR, value);
            ++this->ppuMemory.GetRegisters().name.OAMADDR;
            return;
            break;
        }
        case PpuRegisters::PPUDATA_ADDR:
        {
            //write value to VRAM address
            dword vramAddress = this->ppuMemory.GetVRamAddress(true);
            this->PpuWrite(vramAddress, value);
            this->ppuMemory.GetRegisters().IncrementVRamAddress();
            return;
            break;
        }
        case DmaAddresses::DMA_ADDR:
        {
            // enable DMA
            dword_p startAddress;
            startAddress.val = 0;
            startAddress.upper = value;
            this->dmaBaseAddress = startAddress.val;
            this->dmaAddressOffset = 0;
            this->dmaGoodCycle = false;
            this->dmaActive = true;
            return;
            break;
        }
    }

    if(this->ppuMemory.GetRegisters().Contains(address))
    {
        this->ppuMemory.GetRegisters().Write(address, value);
    }
    else if(this->apuMemory.Contains(address))
    {
        this->apuMemory.Write(address, value);
    }
    else if(this->ControllerMgr.Contains(address)) // must be after apu registers write
    {
        return this->ControllerMgr.Write(address, value);
    }
    else if(this->cpuMemory.Contains(address))
    {
        this->cpuMemory.Write(address, value);
    }
    else if(this->cartridge != NULL && this->cartridge->Contains(address))
    {
        this->cartridge->Write(address, value);
    }
    else
    {
        throw std::out_of_range("DMA: tried to CPU write to an invalid memory address.");
    }
}

byte Dma::Seek(dword address) const
{
    if(this->ControllerMgr.Contains(address))
    {
        return this->ControllerMgr.Seek(address);
    }
    else if(this->ppuMemory.GetRegisters().Contains(address))
    {
        return this->ppuMemory.GetRegisters().Seek(address);
    }
    else if(this->apuMemory.Contains(address))
    {
        return this->apuMemory.Seek(address);
    }
    else if(this->cpuMemory.Contains(address))
    {
        return this->cpuMemory.Seek(address);
    }
    else if(this->cartridge != NULL &&this->cartridge->Contains(address))
    {
        return this->cartridge->Seek(address);
    }
    else
    {
        throw std::out_of_range("DMA: tried to CPU seek to an invalid memory address.");
    }
}

void Dma::ProcessDma()
{
    //handle DMA active
    if(this->IsDmaActive())
    {
        //should write every second ppu cycle
        if(this->GetPpuMemory().GetTotalPpuCycles() % 2 == 0)
        {
            //read the value
            dword fetchAddress = this->GetDmaBaseAddress() + this->GetDmaAddressOffset();
            this->SetDmaBuffer(this->Read(fetchAddress));
        }
        else
        {
            //write the value
            dword targetAddress = this->GetDmaAddressOffset();
            byte bufferVal = this->GetDmaBuffer();
            this->GetPpuMemory().GetPrimaryOam().Write(targetAddress, bufferVal);

            ++targetAddress;
            this->SetDmaAddressOffset(targetAddress);

            if(targetAddress >= 256)
            {
                //reached the end of DMA
                this->dmaActive = false;
            }
        }
    }
}

bool Dma::IsDmaActive()
{
    if(this->GetDmaActive() == false)
    {
        return false;
    }

    if(this->GetDmaGoodCycle() == false)
    {
        //dma starts on the next even cycle
        if(this->GetPpuMemory().GetTotalPpuCycles() % 2 == 1)
        {
            this->SetDmaGoodCycle(true);
        }
        return false;
    }

    return true;
}

byte Dma::PpuRead(dword address)
{
    if(this->cartridge != NULL &&this->cartridge->Contains(address))
    {
        return this->cartridge->Read(address);
    }
    else if(this->ppuMemory.Contains(address))
    {
        return this->ppuMemory.Read(address);
    }
    else
    {
        std::cerr << "DMA: failed to PPU read from address: " << address << std::endl;
    }
    return 0;
}

void Dma::PpuWrite(dword address, byte value)
{
    if(this->cartridge != NULL && this->cartridge->Contains(address))
    {
        this->cartridge->Write(address, value);
    }
    else if(this->ppuMemory.Contains(address))
    {
        this->ppuMemory.Write(address, value);
    }
    else
    {
        std::cerr << "DMA: failed to PPU write to address: " << address << std::endl;
    }
}

CpuMemoryMap& Dma::GetCpuMemory()
{
    return this->cpuMemory;
}

PpuMemoryMap& Dma::GetPpuMemory()
{
    return this->ppuMemory;
}

ApuMemoryMap& Dma::GetApuMemory()
{
    return this->apuMemory;
}

ControllerManager& Dma::GetControllerManager()
{
    return this->ControllerMgr;
}
    

bool Dma::SetCartridge(std::shared_ptr<ICartridge> cartridge)
{
    this->cartridge = cartridge;
    INes::MirrorType mirroringType = this->cartridge->GetMirroringType();
    this->ppuMemory.GetNameTables().SetMirrorType(mirroringType);
    return true;
}

const std::shared_ptr<ICartridge>& Dma::GetCartridge() const
{
    return this->cartridge;
}

PatternTables::BitTile& Dma::GetPatternTile(int tableNum, patternIndex patternNum)
{
    if(this->cartridge == NULL)
    {
        throw std::out_of_range("can't get chr rom data because cartridge is NULL");
    }
    if(tableNum < 0 || tableNum >= PatternTables::NUM_OF_TABLES)
    {
        throw std::out_of_range("table num is not in the boundary");
    }
    std::shared_ptr<MemoryRepeaterVec> chrRom = this->cartridge->GetChrRom();
    std::shared_ptr<std::vector<byte>> chrDataVec = chrRom->GetDataVec();
    byte * chrRawData = chrDataVec->data();
    //do a dynmatic type conversion of the binary
    PatternTables::BitPatternTables& patternTables = (PatternTables::BitPatternTables&) *chrRawData;
    return patternTables.tables[tableNum].raw[patternNum];
}

std::shared_ptr<ICartridge>& Dma::GetCartridge()
{
    return this->cartridge;
}

bool Dma::GetNmi()
{
    return this->nmiActive;
}

void Dma::SetNmi(bool isActive)
{
    this->nmiActive = isActive;
}

bool Dma::GetIrq()
{
    return this->irqActive;
}

void Dma::SetIrq(bool isActive)
{
    this->irqActive = isActive;
}

std::unique_ptr<PatternTables> Dma::GeneratePatternTablesFromRom()
{
    if(this->cartridge == nullptr)
    {
        std::cerr << "can't get chr rom data because cartridge is NULL" << std::endl;
        return nullptr;
    }
    std::shared_ptr<MemoryRepeaterVec> chrRom = this->cartridge->GetChrRom();
    std::shared_ptr<std::vector<byte>> chrDataVec = chrRom->GetDataVec();
    std::unique_ptr<PatternTables> patternTables = std::unique_ptr<PatternTables>( new PatternTables(chrDataVec) );
    return patternTables;
}

bool Dma::GetDmaActive() const
{
    return this->dmaActive;
}

bool Dma::GetDmaGoodCycle() const
{
    return this->dmaGoodCycle;
}

void Dma::SetDmaGoodCycle(bool value)
{
    this->dmaGoodCycle = value;
}

dword Dma::GetDmaBaseAddress()
{
    return this->dmaBaseAddress;
}

dword Dma::GetDmaAddressOffset()
{
    return this->dmaAddressOffset;
}

void Dma::SetDmaAddressOffset(dword address)
{
    this->dmaAddressOffset = address;
}

byte Dma::GetDmaBuffer()
{
    return this->dmaBuffer;
}

void Dma::SetDmaBuffer(byte value)
{
    this->dmaBuffer = value;
}