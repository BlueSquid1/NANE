#include "NES/Memory/BitUtil.h"

#include "NesController.h"

NesController::NesController(dword cpuAddress)
: IMemoryRW(cpuAddress, cpuAddress), 
  keyPressed(8, false),
  readBuffer(8, false)
{

}

void NesController::SetKey(NesInputs& input, bool isPressed)
{
    if(input == NesInputs::UNDEFINED)
    {
        return;
    }
    
    this->keyPressed.at(input) = isPressed;
}

void NesController::Write(dword /*address*/, byte value)
{
    for(int i = 0; i < 8; ++i)
    {
        this->readBuffer.at(i) = this->keyPressed.at(i);
    }
    this->bufferIndex = 0;
    this->isStrobing =  !BitUtil::GetBits(value,0);
}

byte NesController::Read(dword address)
{
    byte readValue = this->Seek(address);
    ++this->bufferIndex;
    return readValue;
}

byte NesController::Seek(dword /*address*/) const
{
    if(this->bufferIndex >= 8 || !this->isStrobing)
    {
        return 0x41;
    }
    byte keyPress = this->readBuffer.at(this->bufferIndex);
    return 0x40 | keyPress;
}

const std::vector<bool>& NesController::GetKeyPressed() const
{
    return this->keyPressed;
}