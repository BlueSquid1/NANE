#include "NesController.h"

NesController::NesController(dword cpuAddress)
: IMemoryRW(cpuAddress, cpuAddress), 
  keyPressed(8, false),
  readBuffer(8, false)
{

}

void NesController::PressKey(NesInputs& input)
{
    if(input == NesInputs::UNDEFINED)
    {
        return;
    }
    
    this->keyPressed.at(input) = true;
}

void NesController::Write(dword address, byte value)
{
    for(int i = 0; i < 8; ++i)
    {
        this->readBuffer.at(i) = this->keyPressed.at(i);
        this->keyPressed.at(i) = false;
    }
    this->bufferIndex = 0;
}

byte NesController::Read(dword address)
{
    if(this->bufferIndex >= 8)
    {
        return 0x1;
    }
    int keyPress = this->readBuffer.at(this->bufferIndex);
    ++this->bufferIndex;
    return keyPress;
}

byte NesController::Seek(dword address) const
{
    // official Nintendo brand controllers always return 1 by default
    return 1;
}